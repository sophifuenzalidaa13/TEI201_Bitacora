# Fuentes de Pulso Ruta

## Librerías usadas

| Librería | Versión | Uso en el proyecto | Fuente |
| --- | --- | --- | --- |
| SparkFun MAX3010x Pulse and Proximity Sensor Library (`MAX30105.h` y `heartRate.h`) | 1.1.2 como referencia de instalación. La versión instalada en el equipo original está pendiente de confirmar. | Configura el MAX30102, lee la señal infrarroja con `getIR()` y detecta latidos con `checkForBeat()`. La usan los sketches con MAX30102 de las versiones local y de nube. | [SparkFun y versión publicada](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library/blob/master/library.properties) |
| `Wire.h` y API PWM LEDC de Arduino-ESP32 | Incluidas en Arduino-ESP32 3.x. La versión menor y el parche instalados están pendientes de confirmar. | `Wire` permite comunicar el ESP32-S3 con el sensor por I²C. `ledcAttach()` y `ledcWriteTone()` controlan el zumbador. | [I²C de Espressif](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html) · [LEDC de Espressif](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ledc.html) |
| `WiFi.h` y `WebServer.h` para el panel local | Incluidas en Arduino-ESP32 3.x. La versión exacta instalada está pendiente de confirmar. | El firmware de la bitácora crea la red Wi-Fi del ESP32 y sirve el panel local con `GET /estado` y `POST /recalibrar`. | [Arduino-ESP32 de Espressif](https://github.com/espressif/arduino-esp32) |
| `WiFi.h`, `WiFiClientSecure.h` y `HTTPClient.h` | Incluidas en Arduino-ESP32 3.x. La versión menor y el parche instalados están pendientes de confirmar. | Conectan el ESP32-S3 a Wi-Fi y permiten enviar las lecturas al servidor por HTTPS. Las usa `cardiaco_nube.ino`. | [Arduino-ESP32 de Espressif](https://github.com/espressif/arduino-esp32) |
| Componentes de plataforma del ESP32-S3 (`time.h`, `sys/time.h`, `esp_timer.h`, `esp_random.h` y FreeRTOS) | Incluidos en la plataforma utilizada por Arduino-ESP32 3.x. | La versión de nube usa las funciones del sistema para mantener la hora de captura, identificar eventos y ejecutar el envío en segundo plano. | [Arduino-ESP32 de Espressif](https://github.com/espressif/arduino-esp32) · [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/) |
| Express | 5.3.0 | Sirve la interfaz web y las rutas de la API desde `server.js`. | [Express](https://expressjs.com/) |
| express-rate-limit | 8.7.5 | Limita solicitudes de inicio de sesión y de envío de lecturas. | [express-rate-limit](https://github.com/express-rate-limit/express-rate-limit) |
| pg (node-postgres) | 8.23.1 | Conecta el servidor a PostgreSQL en Aiven y ejecuta las consultas para guardar o recuperar los datos. | [node-postgres](https://node-postgres.com/) |
| dotenv | 17.4.2 | Carga las variables de entorno del archivo `.env` en el servidor local. | [dotenv](https://github.com/motdotla/dotenv) |

Las versiones exactas del servidor se verificaron en el `package-lock.json` del repositorio de despliegue. El firmware de esta bitácora usa las dependencias descritas en [`software/librerias/librerias.md`](software/librerias/librerias.md). El servidor requiere [Node.js](https://nodejs.org/) 22 o superior. El gráfico del dashboard se dibuja con Canvas, una API del navegador.

## Código externo adaptado

- **SparkFun, ejemplo de frecuencia cardíaca.** El patrón de lectura es el de `Example5_HeartRate.ino`. Se lee la señal infrarroja, se llama a `checkForBeat()` y se calcula el pulso como `60000 / intervalo_en_ms`. La adaptación configura los pines I²C, comprueba el contacto, filtra los valores de BPM y añade calibración personal, promedio móvil, margen de ±20 % y aviso sonoro. La versión de esta bitácora incorpora un panel local y la versión de nube añade el envío por HTTPS. El detector `checkForBeat()` se utiliza desde la librería de SparkFun. [Ejemplo de SparkFun](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library/blob/master/examples/Example5_HeartRate/Example5_HeartRate.ino).
- **Certificados raíz de Let’s Encrypt y Google Trust Services.** En `certificados.h` se reunieron los certificados ISRG Root X1 y X2 y GTS Root R1 a R4 en un bloque PEM que utiliza el cliente HTTPS del ESP32. Se conservaron los certificados y se incorporaron al formato que consume el sketch. [Let’s Encrypt](https://letsencrypt.org/certificates/) · [Google Trust Services](https://pki.goog/repository/).

## Uso de IA

**Herramienta utilizada:** ChatGPT (OpenAI). Las siguientes descripciones sintetizan las solicitudes de desarrollo y señalan qué quedó implementado.

| Solicitud o tarea | Función generada y adaptación al proyecto |
| --- | --- |
| Crear un dashboard para consultar el pulso y las fluctuaciones de distintos usuarios. | ChatGPT ayudó a generar el panel, el servidor Express y el esquema PostgreSQL. La interfaz muestra el pulso actual, los límites, el gráfico y el historial. Cada reloj se asigna a un usuario para consultar sus lecturas. |
| Conectar el ESP32-S3 al dashboard publicado a partir del código del pulsómetro. | ChatGPT preparó `cardiaco_nube.ino` para enviar lecturas por HTTPS a `/api/lecturas`. Se mantuvo la calibración y el zumbador local, se separaron los datos de configuración en `config.h` y se agregó una cola en RAM para cortes breves. El envío corre en una tarea separada del muestreo. |
| Mostrar en qué ruta ocurrió una fluctuación. | Se incorporó el registro de rutas por nombre e intervalo horario. Se compara la hora de captura de cada evento con el inicio y término guardados para el reloj consultado. |
| Preparar `README.md` y `FUENTES.md` para la entrega. | ChatGPT preparó ambos documentos a partir del Avance 1, los sketches, la bitácora y el dashboard publicado. Las versiones del servidor se comprobaron en el archivo de dependencias y las conexiones se cotejaron con las constantes del firmware. |
