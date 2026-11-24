# Iteración 3 · Integración de Pulso Ruta

El **23 de noviembre de 2026** se realizó un ensayo de banco para verificar el flujo desde el sensor hasta el dashboard y diagnosticar problemas de lectura con el montaje. La distancia exacta entre sensor y piel no quedó registrada en la sesión con BPM válidos. Se probaron distintas versiones de firmware durante la jornada.

## Resultados de pruebas

| Prueba | Qué se hizo y qué se observó | Evidencia |
|---|---|---|
| **Captura local de pulso** | Se registraron estimaciones de BPM en el monitor serie del ESP32-S3. En el extracto visible hay nueve valores entre 55,8 y 83,1 BPM, con mediana de 59,7 BPM. | [Monitor serie](../../testing/evidencias/avance-3/monitor-serie.jpg) · [CSV](../../testing/datos/avance-3.csv) |
| **Referencia personal y límites** | Se consultó la referencia calculada en el dashboard. La referencia de 71,5 BPM generó correctamente la banda de ±20 %, de 57,2 a 85,8 BPM. La captura no registra el tiempo de calibración. | [Referencia y rango](../../testing/evidencias/avance-3/dashboard-calibracion.png) |
| **Envío y visualización** | Se compararon las respuestas del monitor con el dashboard durante un ensayo de 33 s. El video muestra respuestas HTTP 201 con BPM, entre ellas 104,6 y 98,0, y actualizaciones visibles en la página. La tasa de entrega y el retardo aún no se midieron. | [Video de 33 s](../../testing/evidencias/avance-3/prueba-envio-dashboard.mp4) |
| **Historial y pérdida de señal** | Se observaron cambios entre «Sin contacto» y «Esperando pulso fiable». El panel conservó la lectura guardada de 59,7 BPM y mostró «Sin señal reciente». | [Registro de estados](../../testing/evidencias/avance-3/dashboard-contacto.jpg) · [Lectura guardada](../../testing/evidencias/avance-3/dashboard-lectura-guardada.png) |
| **Desviaciones visibles** | En el historial se observaron dos registros sobre la banda, de 116,2 y 114,5 BPM. La prueba respalda la indicación visual. La activación del zumbador físico sigue por verificar. | [Eventos fuera de rango](../../testing/evidencias/avance-3/dashboard-fuera-de-rango.jpg) |
| **Diagnóstico de señal óptica** | Se observó saturación al tomar la referencia. Otro ensayo registró contraste negativo y amplitud filtrada de 0,0. Con el filtro continuo apareció un intervalo aislado, pero el contador volvió a cero antes de alcanzar los tres requeridos. | [Saturación](../../testing/evidencias/avance-3/ir-referencia-saturada.webp) · [Contraste negativo](../../testing/evidencias/avance-3/ir-reflejo-negativo.webp) · [Intervalos incompletos](../../testing/evidencias/avance-3/ir-intervalos-incompletos.webp) |
| **Reintentos de conexión** | En una versión de diagnóstico, el cliente informó el error HTTPS −1, un error TLS genérico y un reintento. Hace falta una prueba controlada para medir la recuperación. | [Log del reintento](../../testing/evidencias/avance-3/railway-error-tls.webp) |

El [informe de pruebas](../../testing/reportes/avance-3.md) detalla el protocolo y la relación con el problema de somnolencia en rutas largas. Las pruebas muestran que el sistema puede registrar desviaciones del pulso. Para evaluar su utilidad ante somnolencia se requiere un ensayo específico en condiciones controladas.

## Pruebas pendientes y cómo registrarlas

| Pendiente | Cómo obtener evidencia |
|---|---|
| **Precisión y estabilidad con la carcasa** | Anotar el dispositivo de referencia, la versión del firmware, la posición y la separación real del sensor. En reposo, registrar pares de BPM simultáneos con contacto y con la separación final. Anotar cuándo no hubo lectura y calcular el error absoluto medio y la proporción de lecturas válidas. |
| **Latencia y tasa de entrega** | Guardar un log con ID y hora de captura, respuesta HTTP y hora de recepción para cada evento. Comparar IDs únicos en placa y servidor. Repetir con un corte breve y restablecimiento del hotspot para medir recuperación. |
| **Alarma sonora** | En banco, usar una prueba controlada con tres evaluaciones consecutivas fuera de la banda. Grabar con audio el zumbador y el monitor serie en el mismo video. Registrar en cuál evaluación comienza a sonar y cuándo se apaga al volver al rango. |
| **Asociación de eventos con una ruta** | Registrar en el dashboard una ruta de prueba cuyo intervalo incluya un evento conocido y excluya otro. Guardar capturas de ambos registros y sus horas. |
| **Prueba con usuarios y utilidad frente al problema** | En un escenario simulado y sin conducción, pedir a las personas que identifiquen la alerta y consulten el historial. Guardar fichas anónimas con la tarea, el resultado y su comentario. Evaluar la relación con somnolencia en un estudio aparte. |
| **Fotografías de la carcasa** | Ya hay fotos del [aparato cerrado y puesto](../../hardware/Aparato%20cerrado%20y%20puesto) y del [aparato sin tapa](../../hardware/Aparato%20sin%20tapa). Agregar una vista lateral con escala que muestre la separación entre piel y sensor. |

[Montaje y esquema](../../hardware/README.md) · [Diseño 3D](../../diseno_3d/README.md) · [Índice de evidencias](../../testing/evidencias/README.md)
