#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>
#include <sys/time.h>
#include <esp_timer.h>
#include <esp_random.h>
#include "MAX30105.h"
#include <math.h>

#if __has_include("config.h")
#include "config.h"
#else
#error "Copia config.example.h a config.h y completa los datos del reloj."
#endif
#include "certificados.h"

MAX30105 sensor;
constexpr unsigned long CALIBRACION = 30000;
constexpr float PORCENTAJE = 0.20;
// MAX30102: 400 conversiones/s con promedio de 4 = 100 muestras/s en el FIFO.
constexpr uint8_t CORRIENTE_IR = 0xFF;  // Máxima corriente de pulso programable.
constexpr uint16_t RANGO_ADC_NA = 4096; // Usar 8192 si el montaje satura a 2 mm.
constexpr uint8_t PROMEDIO_FIFO = 4;
constexpr uint16_t FRECUENCIA_ADC = 400;
constexpr unsigned long PERIODO_MUESTRA = 1000UL * PROMEDIO_FIFO / FRECUENCIA_ADC;
constexpr uint32_t IR_SATURADO = 260000; // Margen bajo el máximo de 18 bits.
constexpr uint16_t MUESTRAS_VACIO = 180;
constexpr unsigned long ASENTAMIENTO_IR = 700;
constexpr unsigned long TIEMPO_SIN_PULSO = 3000;
constexpr float REFLEJO_MINIMO = 100.0f;
constexpr float OSCILACION_MINIMA = 6.0f;
constexpr unsigned long INTERVALO_ENVIO = 5000;
constexpr unsigned int COLA_CAPACIDAD = 128;
constexpr unsigned int RESERVA_CAMBIOS = 16;

enum Estado : uint8_t { CALIBRANDO,
                        SIN_CONTACTO,
                        SIN_LECTURA,
                        NORMAL,
                        VARIACION,
                        ALERTA };
enum Direccion : uint8_t { NINGUNA,
                           BAJA,
                           ALTA };
const char* const ESTADOS[] = {
  "calibrando", "sin_contacto", "sin_lectura", "normal", "variacion", "alerta"
};
const char* const DIRECCIONES[] = { "null", "\"baja\"", "\"alta\"" };

struct Lectura {
  int64_t capturaUs;
  uint32_t id;
  Estado estado;
  Direccion direccion;
  bool cambio;
  float bpm;
  float base;
};

QueueHandle_t cola = nullptr;
char idArranque[17];
uint32_t numeroLectura = 0;
unsigned long ultimoEnvio = 0;
Estado estadoEnviado = SIN_CONTACTO;
Direccion direccionEnviada = NINGUNA;
bool primeraLectura = true;

float ultimosBPM[4] = { 0 };
int posicion = 0;
int cantidad = 0;
unsigned long ultimoLatido = 0;
unsigned long ultimaLecturaValida = 0;
unsigned long inicioCalibracion = 0;
float sumaBPM = 0;
int cantidadBPM = 0;
float promedioFijo = 0;
bool calibrado = false;
bool dedoPresente = false;
uint16_t muestrasVacio = 0;
uint64_t sumaIRVacio = 0;
uint32_t minimoIRVacio = IR_SATURADO;
uint32_t maximoIRVacio = 0;
float nivelVacio = 0;
float margenPresencia = REFLEJO_MINIMO;
float ruidoVacio = 0;
float irSuave = 0;
float nivelIR = 0;
float amplitudIR = 0;
bool cruceArmado = false;
unsigned long inicioPresencia = 0;
unsigned long ultimaMuestraIR = 0;
unsigned long ultimoDiagnosticoIR = 0;
int desviaciones = 0;
bool alarmaActiva = false;
Estado estadoActual = SIN_CONTACTO;
Direccion direccionActual = NINGUNA;
float pulsoActual = 0;

void apagarAlarma() {
  if (alarmaActiva) {
    ledcWriteTone(BUZZER_PIN, 0);
    alarmaActiva = false;
  }
}

void encenderAlarma() {
  if (!alarmaActiva) {
    ledcWriteTone(BUZZER_PIN, 2000);
    alarmaActiva = true;
    Serial.println("ALERTA: variacion mayor al 20%");
  }
}

void registrarEstado(unsigned long ahora) {
  if (cola == nullptr) return;
  const bool cambio = primeraLectura || estadoEnviado != estadoActual || direccionEnviada != direccionActual;
  if (!cambio && (ahora - ultimoEnvio < INTERVALO_ENVIO || uxQueueSpacesAvailable(cola) <= RESERVA_CAMBIOS)) return;

  Lectura lectura = {
    esp_timer_get_time(), numeroLectura + 1, estadoActual, direccionActual, cambio,
    pulsoActual, calibrado ? promedioFijo : 0
  };
  if (xQueueSend(cola, &lectura, 0) != pdTRUE) return;
  numeroLectura = lectura.id;
  ultimoEnvio = ahora;
  primeraLectura = false;
  estadoEnviado = estadoActual;
  direccionEnviada = direccionActual;
}

// La red corre en otra tarea para que el muestreo y el zumbador sigan funcionando.
void enviarANube(void*) {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  configTime(0, 0, "pool.ntp.org", "time.google.com");

  Lectura lectura;
  while (true) {
    xQueuePeek(cola, &lectura, portMAX_DELAY);
    if (WiFi.status() != WL_CONNECTED || time(nullptr) < 1700000000) {
      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    }

    // Deriva la hora UTC del instante de captura, incluso si la lectura quedó en cola.
    timeval wall;
    gettimeofday(&wall, nullptr);
    const int64_t capturaUtcUs = int64_t(wall.tv_sec) * 1000000 + wall.tv_usec - (esp_timer_get_time() - lectura.capturaUs);
    const time_t segundos = capturaUtcUs / 1000000;
    tm utc;
    gmtime_r(&segundos, &utc);
    char fecha[21];
    strftime(fecha, sizeof(fecha), "%Y-%m-%dT%H:%M:%S", &utc);

    char bpm[16] = "null";
    char base[16] = "null";
    if (lectura.bpm > 0) snprintf(bpm, sizeof(bpm), "%.1f", lectura.bpm);
    if (lectura.base > 0) snprintf(base, sizeof(base), "%.1f", lectura.base);
    char payload[384];
    snprintf(
      payload, sizeof(payload),
      "{\"reloj\":\"%s\",\"evento\":\"%s-%lu\",\"tomada_en\":\"%s.%03ldZ\","
      "\"estado\":\"%s\",\"bpm\":%s,\"base\":%s,\"direccion\":%s,\"cambio\":%s}",
      DEVICE_ID, idArranque, static_cast<unsigned long>(lectura.id), fecha,
      static_cast<long>((capturaUtcUs / 1000) % 1000), ESTADOS[lectura.estado], bpm,
      base, DIRECCIONES[lectura.direccion], lectura.cambio ? "true" : "false");

    unsigned long espera = 1000;
    while (true) {
      int codigo = -1;
      if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure cliente;
        cliente.setCACert(SERVER_CA);
        cliente.setHandshakeTimeout(5);
        HTTPClient http;
        http.setConnectTimeout(4000);
        http.setTimeout(4000);
        if (http.begin(cliente, SERVER_URL)) {
          http.addHeader("Content-Type", "application/json");
          http.addHeader("Authorization", String("Bearer ") + DEVICE_TOKEN);
          codigo = http.POST(reinterpret_cast<uint8_t*>(payload), strlen(payload));
          http.end();
        }
      }

      if (codigo == 200 || codigo == 201 || codigo == 400) {
        xQueueReceive(cola, &lectura, 0);
        if (codigo == 400) Serial.println("Nube: lectura rechazada (HTTP 400).");
        break;
      }
      Serial.printf("Nube: HTTP %d. Reintentando.\n", codigo);
      vTaskDelay(pdMS_TO_TICKS(codigo == 429 ? 60000 : espera));
      espera = min(espera * 2, 30000UL);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Wire.begin(SDA_PIN, SCL_PIN);
  ledcAttach(BUZZER_PIN, 2000, 8);
  ledcWriteTone(BUZZER_PIN, 0);
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("ERROR: MAX30102 no detectado");
    while (true) delay(1000);
  }
  // El modo de dos LED habilita el canal IR. El LED rojo queda apagado.
  sensor.setup(0, PROMEDIO_FIFO, 2, FRECUENCIA_ADC, 411, RANGO_ADC_NA);
  sensor.setPulseAmplitudeIR(CORRIENTE_IR);
  sensor.clearFIFO();

  // Prefijo por arranque para que un reintento conserve el mismo identificador.
  snprintf(idArranque, sizeof(idArranque), "%08lx%08lx",
           static_cast<unsigned long>(esp_random()), static_cast<unsigned long>(esp_random()));
  cola = xQueueCreate(COLA_CAPACIDAD, sizeof(Lectura));
  if (cola != nullptr && xTaskCreate(enviarANube, "pulso-nube", 10240, nullptr, 1, nullptr) != pdPASS) {
    vQueueDelete(cola);
    cola = nullptr;
  }
  if (cola == nullptr) Serial.println("ERROR: no se pudo iniciar la conexion a la nube");
  Serial.println("Deja el sensor libre unos 2 segundos para medir la referencia IR.");
}

void limpiarPulso() {
  cantidad = 0;
  posicion = 0;
  desviaciones = 0;
  pulsoActual = 0;
  direccionActual = NINGUNA;
  ultimaLecturaValida = 0;
  apagarAlarma();
}

void procesarLatido(float bpm, unsigned long ahora) {
  if (bpm < 40 || bpm > 200) return;
  ultimaLecturaValida = ahora;
  ultimosBPM[posicion] = bpm;
  posicion = (posicion + 1) % 4;
  if (cantidad < 4) cantidad += 1;

  if (!calibrado) {
    if (inicioCalibracion == 0) {
      inicioCalibracion = ahora;
      Serial.println("Calibrando durante 30 segundos...");
    }
    sumaBPM += bpm;
    cantidadBPM += 1;
    if (ahora - inicioCalibracion >= CALIBRACION && cantidadBPM >= 10) {
      promedioFijo = sumaBPM / cantidadBPM;
      calibrado = true;
      estadoActual = SIN_LECTURA;
      Serial.printf("CALIBRACION TERMINADA: %.1f BPM (%.1f a %.1f)\n",
                    promedioFijo, promedioFijo * 0.8, promedioFijo * 1.2);
    }
    return;
  }
  if (cantidad != 4) return;

  float suma = 0;
  for (float lectura : ultimosBPM) suma += lectura;
  pulsoActual = suma / 4;
  Serial.printf("Pulso actual: %.1f BPM\n", pulsoActual);

  const float minimo = promedioFijo * (1 - PORCENTAJE);
  const float maximo = promedioFijo * (1 + PORCENTAJE);
  if (pulsoActual < minimo || pulsoActual > maximo) {
    if (desviaciones < 3) desviaciones += 1;
    direccionActual = pulsoActual < minimo ? BAJA : ALTA;
    estadoActual = desviaciones >= 3 ? ALERTA : VARIACION;
    if (estadoActual == ALERTA) encenderAlarma();
  } else {
    desviaciones = 0;
    apagarAlarma();
    direccionActual = NINGUNA;
    estadoActual = NORMAL;
  }
}

void reiniciarVentanaPulso() {
  ultimoLatido = 0;
  cruceArmado = false;
  amplitudIR = 0;
  limpiarPulso();
  if (!calibrado) {
    inicioCalibracion = 0;
    sumaBPM = 0;
    cantidadBPM = 0;
  }
}

void medirReferenciaIR(uint32_t ir) {
  if (muestrasVacio < 20) {
    irSuave = nivelIR = ir;
  } else {
    sumaIRVacio += ir;
    minimoIRVacio = min(minimoIRVacio, ir);
    maximoIRVacio = max(maximoIRVacio, ir);
    irSuave += 0.25f * (ir - irSuave);
    nivelIR += 0.025f * (irSuave - nivelIR);
    ruidoVacio = fmaxf(ruidoVacio, fabsf(irSuave - nivelIR));
  }
  if (++muestrasVacio == MUESTRAS_VACIO) {
    nivelVacio = float(sumaIRVacio) / (MUESTRAS_VACIO - 20);
    margenPresencia = fmaxf(REFLEJO_MINIMO, 2.0f * (maximoIRVacio - minimoIRVacio));
    irSuave = nivelIR = nivelVacio;
    Serial.printf("Referencia IR: %.0f. Umbral de reflejo: %.0f cuentas.\n",
                  nivelVacio, margenPresencia);
    Serial.println("Acerca el dedo a 2 mm del vidrio y mantenlo quieto.");
  }
}

void procesarMuestraIR(uint32_t ir, unsigned long ahora) {
  if (muestrasVacio < MUESTRAS_VACIO) {
    // Una referencia saturada no sirve. Volver a medir al recuperar señal.
    if (ir >= IR_SATURADO) {
      muestrasVacio = 0;
      sumaIRVacio = 0;
      minimoIRVacio = IR_SATURADO;
      maximoIRVacio = 0;
      ruidoVacio = 0;
      if (ahora - ultimoDiagnosticoIR >= 1000) {
        ultimoDiagnosticoIR = ahora;
        Serial.println("IR saturado al medir la referencia. Reduce la luz directa o aumenta RANGO_ADC_NA.");
      }
    } else {
      medirReferenciaIR(ir);
    }
    return;
  }

  const bool saturado = ir >= IR_SATURADO;
  irSuave += 0.25f * (ir - irSuave);
  const float reflejo = irSuave - nivelVacio;
  const float umbralPresencia = dedoPresente ? 0.5f * margenPresencia : margenPresencia;
  if (ahora - ultimoDiagnosticoIR >= 1000) {
    ultimoDiagnosticoIR = ahora;
    Serial.printf("IR: %lu | reflejo: %.0f | amplitud filtrada: %.1f%s\n",
                  static_cast<unsigned long>(ir), reflejo, amplitudIR,
                  saturado ? " | saturado" : "");
  }
  if (saturado || reflejo < umbralPresencia) {
    if (dedoPresente) reiniciarVentanaPulso();
    dedoPresente = false;
    estadoActual = saturado ? SIN_LECTURA : SIN_CONTACTO;
    return;
  }
  if (!dedoPresente) {
    dedoPresente = true;
    inicioPresencia = ahora;
    estadoActual = calibrado ? SIN_LECTURA : CALIBRANDO;
    Serial.println("Reflejo detectado. Buscando pulso...");
  }
  if (ahora - inicioPresencia < ASENTAMIENTO_IR) {
    nivelIR = irSuave;
    return;
  }

  // Quita el nivel de luz constante y pide un cruce entre dos umbrales.
  // La amplitud reciente ajusta el umbral para pulsos fuertes o débiles.
  nivelIR += 0.025f * (irSuave - nivelIR);
  const float pulsoIR = irSuave - nivelIR;
  amplitudIR = fmaxf(fabsf(pulsoIR), 0.993f * amplitudIR);
  const float umbral = fmaxf(fmaxf(OSCILACION_MINIMA, 3.0f * ruidoVacio), 0.20f * amplitudIR);
  if (pulsoIR < -umbral) cruceArmado = true;
  if (cruceArmado && pulsoIR > umbral) {
    cruceArmado = false;
    const unsigned long intervalo = ahora - ultimoLatido;
    if (ultimoLatido == 0 || intervalo >= 300) {
      if (ultimoLatido != 0 && intervalo <= 1500) procesarLatido(60000.0f / intervalo, ahora);
      ultimoLatido = ahora;
    }
  }
  if (ultimoLatido != 0 && ahora - ultimoLatido > TIEMPO_SIN_PULSO) ultimoLatido = 0;
  if (ultimaLecturaValida != 0 && ahora - ultimaLecturaValida > TIEMPO_SIN_PULSO) {
    reiniciarVentanaPulso();
    estadoActual = calibrado ? SIN_LECTURA : CALIBRANDO;
    Serial.println("Esperando una lectura fiable...");
  }
}

void loop() {
  const uint16_t recibidas = sensor.check();
  const unsigned long ahora = millis();
  const uint8_t muestras = sensor.available();
  if (recibidas) {
    if (recibidas != muestras || (ultimaMuestraIR != 0 && ahora - ultimaMuestraIR > 100)) {
      // Una pausa o el desborde del búfer vuelve inutilizable el intervalo.
      reiniciarVentanaPulso();
      dedoPresente = false;
      estadoActual = SIN_LECTURA;
    }
    ultimaMuestraIR = ahora;
    for (uint8_t pendientes = muestras; pendientes > 0; --pendientes) {
      // SparkFun guarda las muestras nuevas después de la posición de lectura.
      sensor.nextSample();
      procesarMuestraIR(sensor.getFIFOIR(), ahora - (pendientes - 1) * PERIODO_MUESTRA);
    }
  } else if (ultimaMuestraIR != 0 && ahora - ultimaMuestraIR > TIEMPO_SIN_PULSO) {
    reiniciarVentanaPulso();
    dedoPresente = false;
    estadoActual = SIN_LECTURA;
    ultimaMuestraIR = 0;
  }
  registrarEstado(ahora);
  delay(1);
}
