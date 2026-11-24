# Evidencias del Avance 3

Capturas y video registrados el 23 de noviembre de 2026 durante las pruebas de banco de Pulso Ruta. Los registros web usan la hora de Santiago de Chile.

| Archivo | Evidencia |
| --- | --- |
| [Monitor serie](avance-3/monitor-serie.jpg) | Lecturas en BPM, detección y retiro del dedo, espera de una señal fiable y reintentos de envío. |
| [Referencia y rango esperado](avance-3/dashboard-calibracion.png) | Referencia de 71,5 BPM y banda de 57,2 a 85,8 BPM. El estado «Esperando pulso fiable» muestra un guion en la tarjeta de último pulso. |
| [Última lectura guardada](avance-3/dashboard-lectura-guardada.png) | El panel conserva 59,7 BPM y muestra «Sin señal reciente». |
| [Cambios de contacto](avance-3/dashboard-contacto.jpg) | El historial registra transiciones entre «Sin contacto» y «Esperando pulso fiable». |
| [Eventos fuera de rango](avance-3/dashboard-fuera-de-rango.jpg) | El historial marca 114,5 y 116,2 BPM como valores sobre el rango. La ruta aparece como «Sin ruta». |
| [Video del ensayo de banco](avance-3/prueba-envio-dashboard.mp4) | Grabación de 1 min 34 s. Muestra contacto con el sensor, calibración, valores de pulso locales y respuestas de envío. |
| [Contacto con el sensor (0:26)](avance-3/video-contacto-sensor.jpg) | Se ve el montaje y la colocación del dedo sobre el sensor óptico. |
| [Reintentos de envío (1:10)](avance-3/video-reintentos-http.jpg) | Se observan intentos con `HTTP -1` y respuestas `HTTP 201` con `BPM: null`. |
| [Calibración y pulso local (1:19)](avance-3/video-pulso-serial-dashboard.jpg) | El monitor serie muestra la calibración terminada y valores de pulso. La tarjeta de último pulso del dashboard muestra un guion en ese fotograma. |

Las capturas y el video respaldan el [informe de pruebas](../reportes/avance-3.md). Las fotografías del montaje físico corresponden al [entregable de hardware](../../hardware/fotos/).
