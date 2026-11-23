# Pulso Ruta

Pulso Ruta es un prototipo de reloj que estima la frecuencia cardíaca con un ESP32-S3 y un sensor MAX30102. Usa un zumbador para alertar cuando el pulso se mantiene fuera de un margen de ±20 % respecto de la referencia personal.
El sistema ofrece una vista en el teléfono mediante la red local del ESP32 y cuenta con una versión de firmware que envía lecturas a un dashboard web.

## Integrantes

- Sofía Lagos
- Sophia Fuenzalida
- Sofía Campos
- Rafaela Montero

## Problema del Avance 1

Quienes conducen camiones en rutas largas tienen dificultades para mantenerse alerta durante recorridos prolongados, especialmente en tramos monótonos, por la noche o de madrugada. La somnolencia puede aparecer antes de que el conductor perciba claramente el cansancio, lo que dificulta reconocer cuándo conviene detenerse a descansar. Pulso Ruta explora una alerta audible basada en cambios de pulso. Su capacidad para detectar somnolencia queda por validar.

**ODS registrados en la bitácora:** 3 (Salud y bienestar) y 11 (Ciudades y comunidades sostenibles).

### Estado del Proyecto
- **Versión actual:** v10.0
- **Última actualización:** 19_10_2026
- **Estado:** Prototipo final

---

## Estructura del Repositorio
- [`hardware/`](hardware/) contiene esquemas, materiales y fotos.
- [`software/`](software/) contiene código Arduino y documentación técnica.
- [`diseno_3d/`](diseno_3d/) contiene modelos y renders.
- [`testing/`](testing/) contiene evidencias de validación.
- [`documentacion/`](documentacion/) contiene informes y presentación.
- [`iteraciones/`](iteraciones/) registra las versiones del prototipo.

---

## Materiales para replicar el prototipo

| Cantidad | Material | Uso |
| --- | --- | --- |
| 1 | ESP32-S3 Super Mini o placa ESP32-S3 compatible | Lee el sensor, controla el zumbador y se comunica por Wi-Fi. |
| 1 | Módulo MAX30102 | Lee la señal infrarroja para estimar los latidos por minuto (BPM). |
| 1 | Zumbador piezoeléctrico KY-006 | Emite una alerta audible mediante PWM. |
| 1 juego | Protoboard y cables puente | Permiten montar y comprobar el circuito. |
| 1 | Cable USB de datos compatible con la placa | Permite programar y alimentar el ESP32-S3 durante la prueba. |
| Opcional | Batería compatible, carcasa y correa | Permiten usar el montaje en formato portátil. |

| Conexión | Pin en el ESP32-S3 |
| --- | --- |
| SDA del MAX30102 | GPIO4 |
| SCL del MAX30102 | GPIO5 |
| Señal del zumbador | GPIO6 |
| Masa de los módulos | GND común |

Estos pines corresponden a los sketches documentados. Si el montaje usa otros pines, ajusta las constantes antes de cargar el código. Verifica la alimentación de los módulos y si el zumbador requiere una etapa de manejo. [Guía de instalación y conexiones](software/docs/INSTALACION.md).

## Cargar el código y usar el panel local

1. Instala [Arduino IDE 2.x](https://www.arduino.cc/en/software) y, en el Gestor de placas, **esp32 by Espressif Systems** de la rama **3.x**. Si el paquete no aparece, añade el índice oficial en **Preferencias → URLs adicionales del gestor de placas**. [Guía de Espressif](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).
2. En el Gestor de bibliotecas instala **SparkFun MAX3010x Pulse and Proximity Sensor Library**. Las dependencias se detallan en [FUENTES.md](FUENTES.md).
3. Abre [`Monitor_MAX30102_WiFi_KY006_Activo.ino`](software/src/Monitor_MAX30102_WiFi_KY006_Activo.ino). Si Arduino IDE solicita crear la carpeta del sketch, acepta. Conecta el ESP32-S3 por USB, selecciona **ESP32S3 Dev Module** o la definición de tu placa y elige el puerto correspondiente.
4. Revisa las constantes `SDA_PIN`, `SCL_PIN`, `BUZZER_PIN`, `NOMBRE_RED`, `CLAVE_RED` y `ACTIVAR_KY006`. Pulsa **Verificar** y luego **Subir**. Abre el **Monitor Serie a 115200 baudios** y comprueba que se detecte el sensor.
5. En el teléfono, conéctate a la red Wi-Fi del ESP32 (`PulsoESP32` por defecto). Abre en el navegador la dirección HTTP que aparezca en el Monitor Serie. Esta red funciona sin internet.
6. Coloca el dedo sobre el sensor y mantenlo estable. La calibración requiere al menos 30 segundos y 15 mediciones válidas una vez iniciada. El panel muestra el pulso, el promedio de referencia y los límites. Puedes activar el sonido del teléfono o recalibrar desde la misma página.

## Dashboard publicado en Railway

1. Abre [Pulso Ruta](https://pulso-ruta-production.up.railway.app/) e inicia sesión con tu correo y contraseña. Para evaluar el dashboard, solicita al equipo una cuenta con un reloj asignado.
2. Selecciona el reloj y el período que quieres consultar. El panel muestra el pulso en BPM, la referencia, los límites y el historial de fluctuaciones. En **Rutas → Registrar una ruta** puedes asociar eventos a un recorrido por su intervalo horario.
3. Para enviar lecturas nuevas, pide al equipo la carpeta completa de `cardiaco_nube.ino`. Este firmware usa Wi-Fi de **2,4 GHz con internet**. Copia `config.example.h` a `config.h` y configura `WIFI_SSID`, `WIFI_PASSWORD`, `DEVICE_ID` y `DEVICE_TOKEN`. Usa `https://pulso-ruta-production.up.railway.app/api/lecturas` como `SERVER_URL`. La administración entrega el ID y el token del reloj. Guarda las credenciales en tu copia local de `config.h`.
4. Compila y sube `cardiaco_nube.ino` con la misma placa y puerto de Arduino IDE. Coloca el dedo sobre el sensor para calibrar el reloj y selecciona en el panel el reloj asociado al ID configurado. El sketch local `cardiaco.ino` sirve para una prueba por Monitor Serie. El firmware del panel local crea el punto de acceso Wi-Fi del ESP32. Para transmitir a Railway se usa `cardiaco_nube.ino`.

---

## Checklist de Entrega

### Hardware ✓
- [ ] Esquema del circuito (Fritzing/Wokwi)
- [ ] BOM completo
- [ ] Fotos de alta resolución

### Software ✓
- [ ] Código comentado
- [ ] Librerías documentadas
- [ ] Manual de instalación

### Diseño 3D ✓
- [ ] Archivos Fusion 360 (.f3d) con TODOS los componentes
- [ ] Renders de alta calidad (1+ ángulos)
- [ ] Planos técnicos

### Testing ✓
- [ ] Reporte de testing con usuarios (mín. 5)
- [ ] Evidencias fotográficas/videos
- [ ] Datos cuantitativos

### Documentación ✓
- [ ] Reporte final (máx. 15 páginas)
- [ ] Presentación (10 min)

---


---

## Licencia
Este proyecto se desarrolla como parte del curso TEI201 de la Facultad de Ingeniería y Ciencias de la Universidad Adolfo Ibáñez.
