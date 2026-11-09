#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

// ESP32-S3 Super Mini + MAX30102
// Se conecta al telefono SIN internet: red Wi-Fi propia.
// El telefono muestra los datos y puede sonar.
// Control KY-006 activado. Usar solo con la conexion ya verificada.

const char *NOMBRE_RED = "PulsoESP32";
const char *CLAVE_RED = "Pulso2026";  // Minimo 8 caracteres
WebServer servidor(80);
MAX30105 sensor;

const int SDA_PIN = 4;
const int SCL_PIN = 5;
const int BUZZER_PIN = 6;

// KY-006 habilitado tras verificar la conexion y el consumo de la unidad.
const bool ACTIVAR_KY006 = true;
bool buzzerListo = false;
bool pitidoEncendido = false;
unsigned long inicioCicloPitido = 0;
const long UMBRAL_IR = 30000;
const unsigned long TIEMPO_CALIBRACION = 30000;
const float DESVIACION = 0.20f;
const int CONFIRMACIONES = 3;

const int N = 5;
float historial[N] = {0};
int posicion = 0;
int cantidad = 0;
int fueraDeRango = 0;

unsigned long ultimoLatido = 0;
unsigned long ultimoLatidoValido = 0;
unsigned long ultimoDedo = 0;
unsigned long ultimaMuestra = 0;
unsigned long inicioCalibracion = 0;
unsigned long ultimaImpresion = 0;

long irActual = 0;
float bpmActual = 0;
float bpmMovil = 0;
float promedioFijo = 0;
float sumaCalibracion = 0;
int medicionesCalibracion = 0;
bool calibrado = false;
bool dedoPresente = false;
bool alarmaActiva = false;

// Todo el HTML, CSS y JS esta guardado en la placa.
const char PAGINA[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Monitor cardiaco ESP32</title>
<style>
  body {margin:0; background:#0b1220; color:#f8fafc;
        font-family:system-ui,Arial,sans-serif; text-align:center;}
  main {max-width:470px;margin:0 auto;padding:24px 18px;}
  h1 {font-size:24px;margin:12px 0 7px;}
  p {color:#cbd5e1;}
  .card {background:#1e293b;border-radius:18px;padding:21px;
         margin:14px 0;}
  .bpm {font-size:70px;font-weight:750;line-height:1.1;}
  .unit {color:#cbd5e1;font-size:16px;}
  .values {display:flex;justify-content:space-around;gap:8px;}
  .values div {flex:1;}
  .values b {display:block;font-size:25px;margin:7px 0;}
  .values small {color:#cbd5e1;font-size:13px;}
  .state {font-size:18px;font-weight:bold;padding:16px;border-radius:12px;
          background:#334155;}
  .ok {background:#14532d;}
  .alert {background:#991b1b;animation:flash 0.7s infinite alternate;}
  @keyframes flash {from{opacity:1}to{opacity:.65}}
  button {width:100%;border:0;border-radius:12px;padding:16px;
          font-size:18px;font-weight:700;margin:8px 0;
          background:#38bdf8;color:#082f49;}
  button:active {opacity:.8}
  .muted {background:#64748b;color:white;}
  .note {font-size:13px;color:#94a3b8;line-height:1.5;}
  .progress {background:#334155;border-radius:10px;height:12px;overflow:hidden;margin-top:12px;}
  .progress > div {background:#38bdf8;height:100%;width:0%;transition:width .3s;}
</style>
</head>
<body>
<main>
  <h1>Monitor cardiaco</h1>
  <p>ESP32-S3 + MAX30102</p>
  <div class="card">
    <div class="bpm" id="bpm">--</div><div class="unit">Pulsaciones por minuto (BPM)</div>
  </div>
  <div class="card values">
    <div><small>Minimo</small><b id="minimo">--</b></div>
    <div><small>Base fija</small><b id="base">--</b></div>
    <div><small>Maximo</small><b id="maximo">--</b></div>
  </div>
  <div id="estado" class="state">Conectando...</div>
  <div class="progress"><div id="barra"></div></div>
  <p id="detalle">Esperando conexión con la ESP32...</p>
  <button id="activar" onclick="habilitarAudio()">Activar sonido y probar pitido</button>
  <button class="muted" onclick="silenciar()">Silenciar el teléfono</button>
  <button class="muted" onclick="recalibrar()">Recalibrar para otra persona</button>
  <p id="infoAudio">Sonido desactivado</p>
  <p class="note">Conecta el teléfono al Wi-Fi PulsoESP32 y deja esta página abierta.
  Mantén la pantalla encendida si quieres alarma en el teléfono.
  El zumbador físico suena 3 segundos y descansa 1 segundo durante la alerta.
  Prototipo educativo, no es un dispositivo médico.</p>
</main>
<script>
let audio = null;
let sonidoHabilitado = false;
let ultimoPitido = 0;
let consultando = false;
const $ = id => document.getElementById(id);

function pitido() {
  if (!sonidoHabilitado || !audio || audio.state !== 'running') return;
  const t = audio.currentTime;
  const osc = audio.createOscillator();
  const gan = audio.createGain();
  osc.type = 'square';
  osc.frequency.value = 950;
  gan.gain.setValueAtTime(0.075, t);
  gan.gain.exponentialRampToValueAtTime(0.001, t + 0.27);
  osc.connect(gan);
  gan.connect(audio.destination);
  osc.start(t);
  osc.stop(t + 0.28);
}

async function habilitarAudio() {
  try {
    if (!audio) audio = new (window.AudioContext || window.webkitAudioContext)();
    await audio.resume();
    sonidoHabilitado = true;
    $('infoAudio').textContent = 'Sonido activado';
    $('activar').textContent = 'Probar sonido nuevamente';
    pitido();
  } catch (error) {
    $('infoAudio').textContent = 'No se pudo activar: ' + error.message;
  }
}

function silenciar() {
  sonidoHabilitado = false;
  $('infoAudio').textContent = 'Sonido desactivado';
}

function valor(n) {return Number(n).toFixed(1);}

async function recalibrar() {
  if (!confirm('¿Borrar el promedio fijo y comenzar una nueva calibración?')) return;
  try {
    const r = await fetch('/recalibrar', {method:'POST',cache:'no-store'});
    if (!r.ok) throw new Error('No se pudo recalibrar');
    ultimoPitido = 0;
    actualizar();
  } catch (e) {
    alert('No se pudo comunicar con la ESP32. Comprueba la conexión Wi-Fi.');
  }
}

async function actualizar() {
  if (consultando) return;
  consultando = true;
  try {
    const respuesta = await fetch('/estado', {cache:'no-store'});
    if (!respuesta.ok) throw new Error('Sin respuesta');
    const d = await respuesta.json();
    $('base').textContent = d.calibrado ? valor(d.base) : '--';
    $('minimo').textContent = d.calibrado ? valor(d.minimo) : '--';
    $('maximo').textContent = d.calibrado ? valor(d.maximo) : '--';
    $('bpm').textContent = (d.dedo && d.bpm > 0 && d.datos) ? valor(d.bpm) : '--';
    $('barra').style.width = d.calibrado ? '100%' : (d.segundos < 0 ? '0%' :
      String(Math.min(100, Math.round(d.segundos * 100 / 30))) + '%');
    $('detalle').textContent = d.calibrado ?
      'Pulso actual frente al promedio fijo. Alarma después de 3 mediciones fuera del ±20%.' :
      (d.segundos < 0 ? 'Coloca el dedo y espera cinco latidos válidos.' :
      'Calibración en curso: ' + Math.min(d.segundos, 30) + ' de 30 segundos.');
    const estado = $('estado');
    estado.className = 'state';
    if (!d.datos) {
      estado.textContent = 'SIN DATOS DEL SENSOR';
    } else if (!d.dedo) {
      estado.textContent = 'COLOCA EL DEDO';
    } else if (!d.calibrado) {
      estado.textContent = d.segundos < 0 ? 'Buscando latidos...' :
        'CALIBRANDO: ' + d.segundos + ' / 30 s';
    } else if (d.alarma) {
      estado.classList.add('alert');
      estado.textContent = 'ALERTA: PULSO FUERA DEL 20%';
    } else {
      estado.classList.add('ok');
      estado.textContent = 'MONITOREO NORMAL';
    }
    if (d.alarma && sonidoHabilitado && Date.now() - ultimoPitido >= 850) {
      ultimoPitido = Date.now();
      pitido();
    }
  } catch(e) {
    $('estado').className = 'state';
    $('estado').textContent = 'SIN CONEXION CON LA ESP32';
    $('bpm').textContent = '--';
    $('detalle').textContent = 'Revisa la conexión Wi-Fi PulsoESP32.';
  } finally {
    consultando = false;
  }
}
actualizar();
setInterval(actualizar, 650);
</script>
</body>
</html>
)HTML";

// Sonido fisico no bloqueante: 3 segundos encendido y 1 apagado.
// No afecta a las lecturas del sensor ni a las peticiones HTTP.
void actualizarZumbador() {
  if (!buzzerListo) return;
  if (!alarmaActiva) {
    if (pitidoEncendido) {
      ledcWriteTone(BUZZER_PIN, 0);
      pitidoEncendido = false;
    }
    return;
  }

  const unsigned long fase = (millis() - inicioCicloPitido) % 4000;
  const bool debeSonar = fase < 3000;
  if (debeSonar != pitidoEncendido) {
    ledcWriteTone(BUZZER_PIN, debeSonar ? 2000 : 0);
    pitidoEncendido = debeSonar;
  }
}

void apagarAlarma() {
  if (alarmaActiva) {
    alarmaActiva = false;
    Serial.println("ALERTA FINALIZADA");
  }
  actualizarZumbador();
}

void activarAlarma() {
  if (!alarmaActiva) {
    alarmaActiva = true;
    inicioCicloPitido = millis();
    Serial.println("ALERTA: PULSO FUERA DE RANGO");
  }
  actualizarZumbador();
}

void limpiarLatidos() {
  ultimoLatido = 0;
  ultimoLatidoValido = 0;
  cantidad = 0;
  posicion = 0;
  bpmActual = 0;
  bpmMovil = 0;
  fueraDeRango = 0;
  apagarAlarma();
}

void reiniciarCalibracion() {
  inicioCalibracion = 0;
  sumaCalibracion = 0;
  medicionesCalibracion = 0;
}

void enviarEstado() {
  const unsigned long ahora = millis();
  bool hayDatos = (ahora - ultimaMuestra <= 2000);
  String respuesta = "{";
  respuesta += "\"ir\":";
  respuesta += String(irActual);
  respuesta += ",\"bpm\":";
  respuesta += String(bpmMovil, 1);
  respuesta += ",\"base\":";
  respuesta += String(promedioFijo, 1);
  respuesta += ",\"minimo\":";
  respuesta += String(promedioFijo * 0.8f, 1);
  respuesta += ",\"maximo\":";
  respuesta += String(promedioFijo * 1.2f, 1);
  respuesta += ",\"calibrado\":";
  respuesta += (calibrado ? "true" : "false");
  respuesta += ",\"dedo\":";
  respuesta += (dedoPresente ? "true" : "false");
  respuesta += ",\"alarma\":";
  respuesta += (alarmaActiva && hayDatos && dedoPresente ? "true" : "false");
  respuesta += ",\"datos\":";
  respuesta += (hayDatos ? "true" : "false");
  respuesta += ",\"segundos\":";
  respuesta += (inicioCalibracion == 0 ? "-1" : String((ahora - inicioCalibracion) / 1000));
  respuesta += "}";
  servidor.sendHeader("Cache-Control", "no-store");
  servidor.send(200, "application/json", respuesta);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (ACTIVAR_KY006) {
    // Requiere Arduino-ESP32 3.x (ESP32S3 Dev Module).
    buzzerListo = ledcAttach(BUZZER_PIN, 2000, 8);
    if (buzzerListo) ledcWriteTone(BUZZER_PIN, 0);
    else Serial.println("ERROR: PWM GPIO6 no disponible");
  }
  Serial.println("Buscando MAX30102...");
  if (!sensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("ERROR: Sensor no detectado. Revisar cables.");
    while (true) delay(1000);
  }

  // Configuracion que ya funciono con tu MAX30102
  sensor.setup(0x1F, 1, 2, 100, 411, 4096);
  sensor.setPulseAmplitudeRed(0x0A);
  sensor.setPulseAmplitudeIR(0x1F);
  sensor.setPulseAmplitudeGreen(0);

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(NOMBRE_RED, CLAVE_RED)) {
    Serial.println("ERROR: no se pudo crear la red Wi-Fi");
    while (true) delay(1000);
  }

  servidor.on("/", HTTP_GET, []() {
    servidor.send_P(200, "text/html; charset=utf-8", PAGINA);
  });
  servidor.on("/estado", HTTP_GET, enviarEstado);
  servidor.on("/recalibrar", HTTP_POST, []() {
    limpiarLatidos();
    calibrado = false;
    promedioFijo = 0;
    dedoPresente = false;
    reiniciarCalibracion();
    Serial.println("RECALIBRACION SOLICITADA DESDE EL TELEFONO");
    servidor.send(200, "application/json", "{\"ok\":true}");
  });
  servidor.begin();

  ultimaMuestra = millis();
  Serial.println("MAX30102 conectado");
  Serial.print("Red Wi-Fi: "); Serial.println(NOMBRE_RED);
  Serial.print("Clave: "); Serial.println(CLAVE_RED);
  Serial.print("Abre en el telefono: http://"); Serial.println(WiFi.softAPIP());
  Serial.println("Coloca el dedo y activa el sonido desde el telefono");
  Serial.print("KY-006 conectado por software: ");
  Serial.println(buzzerListo ? "SI" : "NO (error de PWM)");
}

void loop() {
  servidor.handleClient();
  actualizarZumbador();

  const long ir = sensor.getIR();
  const unsigned long ahora = millis();

  if (ir == 0) {
    // La biblioteca devuelve 0 si no pudo obtener una muestra.
    // No confundir este caso con retirar el dedo.
    if (ahora - ultimaMuestra > 2000) {
      if (dedoPresente) {
        dedoPresente = false;
        limpiarLatidos();
        if (!calibrado) reiniciarCalibracion();
      }
    }
    delay(3);
    return;
  }

  ultimaMuestra = ahora;
  irActual = ir;

  if (ir < UMBRAL_IR) {
    if (dedoPresente && ahora - ultimoDedo > 1500) {
      dedoPresente = false;
      limpiarLatidos();
      if (!calibrado) reiniciarCalibracion();
      Serial.println("Dedo retirado");
    }
    delay(3);
    return;
  }

  ultimoDedo = ahora;
  if (!dedoPresente) {
    dedoPresente = true;
    limpiarLatidos();
    Serial.println("Dedo detectado. Buscando latidos...");
  }

  if (checkForBeat(ir)) {
    if (ultimoLatido != 0) {
      unsigned long intervalo = ahora - ultimoLatido;
      if (intervalo >= 333 && intervalo <= 1500) {
        bpmActual = 60000.0f / intervalo;
        ultimoLatidoValido = ahora;
        historial[posicion] = bpmActual;
        posicion = (posicion + 1) % N;
        if (cantidad < N) cantidad++;

        float suma = 0;
        for (int i = 0; i < cantidad; i++) suma += historial[i];
        bpmMovil = suma / cantidad;

        if (cantidad == N) {
          if (!calibrado) {
            if (inicioCalibracion == 0) {
              inicioCalibracion = ahora;
              Serial.println("CALIBRACION INICIADA: 30 SEGUNDOS");
            }
            sumaCalibracion += bpmMovil;
            medicionesCalibracion++;
            if (ahora - inicioCalibracion >= TIEMPO_CALIBRACION && medicionesCalibracion >= 15) {
              promedioFijo = sumaCalibracion / medicionesCalibracion;
              calibrado = true;
              Serial.println("CALIBRACION COMPLETA");
              Serial.print("PROMEDIO FIJO: "); Serial.println(promedioFijo, 1);
              Serial.print("LIMITE INFERIOR: "); Serial.println(promedioFijo * 0.8f, 1);
              Serial.print("LIMITE SUPERIOR: "); Serial.println(promedioFijo * 1.2f, 1);
            }
          } else {
            float minimo = promedioFijo * (1.0f - DESVIACION);
            float maximo = promedioFijo * (1.0f + DESVIACION);
            if (bpmMovil < minimo || bpmMovil > maximo) {
              fueraDeRango++;
              if (fueraDeRango >= CONFIRMACIONES) activarAlarma();
            } else {
              fueraDeRango = 0;
              apagarAlarma();
            }
          }
        }
      }
    }
    ultimoLatido = ahora;
  }

  if (ultimoLatidoValido != 0 && ahora - ultimoLatidoValido > 4000) {
    limpiarLatidos();
    if (!calibrado) reiniciarCalibracion();
    Serial.println("Esperando una lectura estable");
  }

  if (ahora - ultimaImpresion >= 1500) {
    ultimaImpresion = ahora;
    Serial.print("IR: "); Serial.print(irActual);
    Serial.print(" | BPM: "); Serial.print(bpmMovil, 1);
    Serial.print(" | BASE: "); Serial.print(promedioFijo, 1);
    Serial.print(" | "); Serial.println(alarmaActiva ? "ALERTA" : (calibrado ? "NORMAL" : "CALIBRANDO"));
  }
  delay(3);
}
