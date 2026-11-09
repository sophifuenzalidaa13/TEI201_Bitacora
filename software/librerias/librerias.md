# Dependencias del proyecto

## Bibliotecas incluidas con el núcleo ESP32
- `WiFi.h`: creación del punto de acceso local.
- `WebServer.h`: servidor HTTP integrado.
- `Wire.h`: comunicación I²C con MAX30102.

Estas cabeceras se obtienen al instalar el paquete de placas `esp32` de **Espressif Systems** desde el gestor de placas de Arduino IDE. La API PWM `ledcAttach()` usada por el sketch corresponde a la rama 3.x de Arduino-ESP32; se recomienda registrar la versión exacta instalada cuando se valide el montaje.

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

## Registro de versiones reales (completar tras instalar)
| Componente | Versión instalada |
|---|---|
| Arduino IDE | Pendiente de registrar |
| Paquete de placas Espressif ESP32 (3.x) | Pendiente de registrar |
| SparkFun MAX3010x | Pendiente de registrar |

**Nota:** `MAX30105.h` es el nombre de la cabecera de SparkFun, pero la misma biblioteca es compatible con MAX30102.
