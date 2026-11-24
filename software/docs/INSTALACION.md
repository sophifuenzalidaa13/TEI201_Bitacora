# Instalación, conexiones y uso

## Hardware requerido
- ESP32-S3 Super Mini.
- Sensor óptico MAX30102.
- Zumbador KY-006, condicionado a verificar su compatibilidad eléctrica y consumo.
- Cables de conexión, alimentación USB y una red Wi-Fi de 2,4 GHz con internet.

## Conexiones de la plantilla `config.example.h`
| Señal | ESP32-S3 | Destino |
|---|---|---|
| SDA | GPIO4 | SDA del MAX30102 |
| SCL | GPIO5 | SCL del MAX30102 |
| PWM zumbador | GPIO6 | Entrada de control del KY-006 según circuito validado |
| GND | GND | Masa común entre módulos |

**Atención:** la alimentación eléctrica del MAX30102 y del zumbador debe adecuarse a las especificaciones **del módulo físico utilizado**, no solo al nombre comercial del sensor. Antes de conectar, verificar su voltaje permitido, corriente, disposición real de pines y si el KY-006 requiere una etapa de manejo. No asumir que GPIO6 puede alimentar directamente cualquier zumbador. Consulta el [esquema del cableado del prototipo](../../hardware/esquemas/README.md).

## Preparar Arduino IDE y cargar el firmware

1. Instala Arduino IDE 2.x. En el Gestor de placas instala **esp32 by Espressif Systems** de la rama **3.x**, que incluye `ledcAttach` para controlar el buzzer.
2. En el Gestor de bibliotecas instala **SparkFun MAX3010x Pulse and Proximity Sensor Library**. El sketch usa `MAX30105.h` y `heartRate.h` de esa biblioteca. Consulta [FUENTES.md](../../FUENTES.md) para ver las versiones registradas.
3. Crea una carpeta llamada `cardiaco_nube`. Guarda ahí el archivo entregado `cardiaco_nube (1).ino` con el nombre `cardiaco_nube.ino`. Descarga [`config.example.h`](../../hardware/config.example.h) y [`certificados.h`](../../hardware/certificados.h) en la misma carpeta.
4. Copia `config.example.h` como `config.h`. Completa `WIFI_SSID` y `WIFI_PASSWORD` con los datos de una red de **2,4 GHz con internet**. Pide al equipo el `DEVICE_ID` y el `DEVICE_TOKEN` del reloj. Conserva el `SERVER_URL` de la plantilla, que apunta a `/api/lecturas` en Railway. Guarda `config.h` solo en tu equipo.
5. Revisa que `SDA_PIN`, `SCL_PIN` y `BUZZER_PIN` en `config.h` coincidan con el cableado. Abre `cardiaco_nube.ino` en Arduino IDE, conecta la placa por USB y elige **ESP32S3 Dev Module** o la definición correspondiente a tu placa. Selecciona el puerto, pulsa **Verificar** y luego **Subir**.
6. Abre el **Monitor Serie a 115200 baudios**. Cuando aparezca `Coloca el dedo sobre el sensor`, comienza la medición.

El procedimiento de medición corresponde al archivo entregado, que usa un umbral infrarrojo fijo. Si cargas el [sketch publicado en la bitácora](../../hardware/cardiaco_nube.ino), deja el sensor despejado durante su ajuste inicial de señal infrarroja y sigue los mensajes del Monitor Serie.

## Medición y alarma del buzzer

1. Coloca el dedo sobre el MAX30102 y mantenlo quieto. La calibración empieza con la primera estimación válida del pulso y termina tras **al menos 30 segundos y 10 mediciones válidas**. El Monitor Serie muestra la referencia personal y sus límites.
2. Una vez calibrado, el firmware promedia las últimas cuatro estimaciones válidas de BPM. El **buzzer físico del reloj** suena tras **tres estimaciones procesadas consecutivas** fuera de la banda de **±20 %** de la referencia. Se apaga cuando el pulso vuelve a la banda, cuando retiras el dedo o cuando pasan más de tres segundos sin una lectura válida.
3. Para calibrar el reloj para otra persona, reinicia el ESP32-S3 y repite la medición inicial.

## Consultar el historial en Railway

1. Abre [Pulso Ruta](https://pulso-ruta-production.up.railway.app/) desde un teléfono o computador con internet. Inicia sesión con una cuenta que tenga el reloj asignado.
2. Selecciona el reloj y el período que quieres consultar. El panel muestra la evolución histórica del pulso en BPM, la referencia, los límites y los cambios de estado. La alerta sonora sale exclusivamente del buzzer del reloj.
3. El ESP32-S3 encola actualizaciones aproximadamente cada **5 segundos** y cuando cambia el estado o la dirección de una variación. El envío HTTPS se ejecuta en una tarea independiente de la medición y del buzzer. Tras sincronizar el reloj, el firmware usa la hora de captura de cada lectura, incluso si quedó pendiente de envío. Si la cola se llena, puede haber intervalos sin datos en el historial.

## Diagnóstico básico

- **MAX30102 no detectado.** Revisa SDA, SCL, alimentación y GND. Confirma que los pines de `config.h` correspondan al montaje.
- **El Monitor Serie no muestra BPM.** Mantén el dedo quieto sobre el sensor y espera a que termine la calibración. El archivo entregado usa `IR_MIN = 50000` para detectar contacto.
- **El historial no se actualiza.** Comprueba la conexión Wi-Fi a internet, las credenciales del reloj y el período seleccionado. Revisa los mensajes `Nube: HTTP ...` en el Monitor Serie.
- **El buzzer no suena.** Verifica el cableado y `BUZZER_PIN`. La alarma audible comienza al entrar en el estado `alerta`.

**Seguridad:** prototipo educativo, no aprobado para decisiones médicas. No confiar en las alarmas para supervisión de salud.
