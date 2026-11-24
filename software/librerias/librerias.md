# Dependencias del proyecto

## Bibliotecas incluidas con el núcleo ESP32
- `WiFi.h`: creación del punto de acceso local.
- `WebServer.h`: servidor HTTP integrado.
- `Wire.h`: comunicación I²C con MAX30102.

Estas cabeceras se obtienen al instalar el paquete de placas `esp32` de **Espressif Systems** desde el gestor de placas de Arduino IDE. La API PWM `ledcAttach()` usada por el sketch está disponible en la versión **3.3.12** de Arduino-ESP32 que se verificó en la compilación.

## Biblioteca externa necesaria
**SparkFun MAX3010x Pulse and Proximity Sensor Library**, de SparkFun Electronics.
- Cabeceras utilizadas: `MAX30105.h` y `heartRate.h`.
- Uso: lectura del MAX30102 y detección de latidos mediante `checkForBeat()`.
- Repositorio oficial: https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library

### Instalación desde Arduino IDE
1. Abrir Arduino IDE.
2. Ir a **Herramientas > Administrar bibliotecas** (o el icono de Bibliotecas).
3. Buscar `SparkFun MAX3010x Pulse and Proximity Sensor Library`.
4. Seleccionar la biblioteca de SparkFun Electronics y presionar **Instalar**.
5. Revisar que las cabeceras del sketch se reconozcan al compilar.

No se requiere copiar código ajeno a esta carpeta si la dependencia se instala desde el gestor. Si más adelante se incorporan bibliotecas personalizadas, documentar nombre, versión, licencia, origen y procedimiento de instalación.

## Versiones verificadas en la compilación del 23 de noviembre de 2026
| Componente | Versión instalada |
|---|---|
| Paquete de placas `esp32` de Espressif Systems | 3.3.12 |
| SparkFun MAX3010x | 1.1.2 |

**Nota:** `MAX30105.h` es el nombre de la cabecera de SparkFun, pero la misma biblioteca es compatible con MAX30102.
