# Instalación, conexiones y uso

## Hardware requerido
- ESP32-S3 Super Mini.
- Sensor óptico MAX30102.
- Zumbador KY-006, condicionado a verificar su compatibilidad eléctrica y consumo.
- Cables de conexión, alimentación USB y teléfono con Wi-Fi.

## Conexiones declaradas en el firmware
| Señal | ESP32-S3 | Destino |
|---|---|---|
| SDA | GPIO4 | SDA del MAX30102 |
| SCL | GPIO5 | SCL del MAX30102 |
| PWM zumbador | GPIO6 | Entrada de control del KY-006 según circuito validado |
| GND | GND | Masa común entre módulos |

**Atención:** la alimentación eléctrica del MAX30102 y del zumbador debe adecuarse a las especificaciones **del módulo físico utilizado**, no solo al nombre comercial del sensor. Antes de conectar, verificar su voltaje permitido, corriente, disposición real de pines y si el KY-006 requiere una etapa de manejo. No asumir que GPIO6 puede alimentar directamente cualquier zumbador. Agregar a `hardware/` una fotografía o diagrama del cableado realmente probado.

## Preparar Arduino IDE
1. Instalar Arduino IDE 2.x.
2. Desde **Herramientas > Placa > Gestor de placas**, instalar `esp32` de Espressif Systems (rama 3.x, que admite `ledcAttach`).
3. Instalar `SparkFun MAX3010x Pulse and Proximity Sensor Library` desde el gestor de bibliotecas.
4. Abrir `src/Monitor_MAX30102_WiFi_KY006_Activo.ino` desde Arduino IDE (si pide crear una carpeta del sketch, aceptar).
5. Elegir una definición de placa compatible con la unidad; el código propone `ESP32S3 Dev Module`, y escoger el puerto correcto.
6. Revisar `SDA_PIN`, `SCL_PIN`, `BUZZER_PIN`, `NOMBRE_RED`, `CLAVE_RED` y `ACTIVAR_KY006` antes de cargar. **No publicar credenciales reales o privadas.**
7. Pulsar **Verificar/Compilar**. Corregir errores antes de cargar; después presionar **Subir**.
8. Abrir monitor serie a **115200 baudios** y confirmar detección del MAX30102 y dirección IP del punto de acceso.

## Uso desde el teléfono
1. Alimentar la placa y observar que se inicie la red local `PulsoESP32` (nombre editable en el sketch).
2. Conectar el teléfono a dicha red Wi-Fi con la clave configurada. La red puede aparecer como **sin Internet**, lo cual es normal.
3. En un navegador, introducir la dirección HTTP que muestre el monitor serie para `WiFi.softAPIP()`.
4. Colocar el dedo sobre el sensor y mantenerlo estable. Primero se obtiene un historial de cinco BPM válidos; luego comienza la calibración de 30 segundos, que requiere al menos 15 mediciones válidas.
5. La página muestra BPM, base fija, mínimos y máximos; presionar **Activar sonido y probar pitido** para habilitar audio del teléfono.
6. Usar **Recalibrar para otra persona** para reiniciar la referencia. **Silenciar el teléfono** solo silencia el navegador; no apaga el zumbador KY-006 físico.

## Diagnóstico básico
- **Sensor no detectado:** comprobar conexiones I²C, alimentación, GND y tipo de módulo; revisar monitor serie.
- **No aparece la red Wi-Fi:** verificar carga, alimentación e inicialización del ESP32.
- **No hay BPM:** colocar dedo correctamente, evitar movimiento y revisar valor IR/umbral.
- **No suena el teléfono:** habilitar sonido mediante interacción con el botón; mantener página activa.
- **No suena el KY-006:** verificar circuito, GPIO6, `ACTIVAR_KY006` y mensaje de disponibilidad de PWM en monitor serie.

**Seguridad:** prototipo educativo, no aprobado para decisiones médicas. No confiar en las alarmas para supervisión de salud.
