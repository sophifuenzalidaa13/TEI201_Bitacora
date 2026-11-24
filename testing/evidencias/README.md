# Evidencias del Avance 3

Capturas y video registrados el 23 de noviembre de 2026 durante las pruebas de banco de Pulso Ruta. Los registros web usan la hora de Santiago de Chile.

| Archivo | Evidencia |
| --- | --- |
| [Monitor serie](avance-3/monitor-serie.jpg) | Lecturas en BPM, detección y retiro del dedo, espera de una señal fiable y reintentos de envío. |
| [Referencia y rango esperado](avance-3/dashboard-calibracion.png) | Referencia de 71,5 BPM y banda de 57,2 a 85,8 BPM. El estado «Esperando pulso fiable» muestra un guion en la tarjeta de último pulso. |
| [Última lectura guardada](avance-3/dashboard-lectura-guardada.png) | El panel conserva 59,7 BPM y muestra «Sin señal reciente». |
| [Cambios de contacto](avance-3/dashboard-contacto.jpg) | El historial registra transiciones entre «Sin contacto» y «Esperando pulso fiable». |
| [Eventos fuera de rango](avance-3/dashboard-fuera-de-rango.jpg) | El historial marca 114,5 y 116,2 BPM como valores sobre el rango. La ruta aparece como «Sin ruta». |
| [Video de envío al dashboard](avance-3/prueba-envio-dashboard.mp4) | En 34 segundos se observan lecturas en el monitor serie, varias respuestas `HTTP 201` y cambios en la tarjeta de último pulso de Railway. |
| [Referencia IR saturada](avance-3/ir-referencia-saturada.webp) | La inicialización óptica detectó saturación. En el mismo extracto se observa un HTTP 201, compatible con el envío de un estado. |
| [Contraste IR negativo](avance-3/ir-reflejo-negativo.webp) | Cuatro registros muestran contraste entre −12.783 y −26.122 cuentas y amplitud filtrada de 0,0. |
| [Intervalos incompletos](avance-3/ir-intervalos-incompletos.webp) | El contador sube a un intervalo y vuelve a cero sin completar la secuencia requerida. |
| [Reintento HTTPS](avance-3/railway-error-tls.webp) | El cliente informó HTTP −1 y TLS −1 genérico, y anunció un reintento. |

Los cuatro recortes muestran solo el área de resultados del monitor serie.

Las capturas y el video respaldan el [informe de pruebas](../reportes/avance-3.md). Las fotografías del montaje físico corresponden al [entregable de hardware](../../hardware/fotos/).
