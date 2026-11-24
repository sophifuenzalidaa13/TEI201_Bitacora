# Avance 3 · Pruebas de Pulso Ruta

**Fecha del ensayo:** 23 de noviembre de 2026.  
**Condiciones documentadas:** ensayo de banco, Arduino IDE 2.3.6, placa configurada como ESP32S3 Dev Module, monitor serie en COM8 a 115200 baudios, y panel de Railway con el reloj de prueba `reloj-01`. Las capturas corresponden a una versión de prueba de `cardiaco_nube.ino`. El código publicado posteriormente ese día muestra mensajes de detección IR diferentes. La distancia al sensor no quedó registrada.

## Objetivo y protocolo reproducible

Comprobar que el prototipo estima el pulso, calcula una referencia personal y permite consultar lecturas y cambios de estado en la nube. Usar las [instrucciones de instalación](../../README.md#dashboard-publicado-en-railway).

| Prueba | Procedimiento | Criterio de éxito |
| --- | --- | --- |
| Captura y referencia | Iniciar el firmware de nube con el sensor despejado hasta que complete la referencia IR. Presentar el dedo según las indicaciones del monitor y mantenerlo quieto. Registrar los BPM y la referencia personal. | Se observan valores numéricos y una referencia tras al menos 30 s desde el inicio de la calibración y 10 estimaciones válidas. Los límites corresponden al 80 % y 120 % de la referencia. |
| Envío e historial | Abrir el reloj configurado en Railway. Registrar hora y BPM de una lectura nueva y volver a consultar el historial tras recargar. | La lectura aparece con hora y valor, y sigue disponible tras recargar. Registrar el código de respuesta y los reintentos para cada envío medido. |
| Pérdida de señal | Retirar y volver a presentar el dedo. En otro ensayo, interrumpir temporalmente la conexión y observar el reloj en el panel. | Ante pérdida de contacto o señal óptica insuficiente aparece el estado correspondiente sin un BPM nuevo. Si dejan de llegar eventos, el panel indica «Sin señal reciente». |
| Umbral y zumbador | En banco, usar una prueba controlada para producir tres valores procesados consecutivos fuera de la banda. Registrar el estado del panel y el sonido del zumbador. | El panel marca la dirección de la desviación y el zumbador se activa al llegar la tercera lectura consecutiva fuera de banda. |
| Asociación a ruta | Registrar una ruta de prueba para el reloj con un intervalo que contenga un evento ya guardado. Volver a consultar el historial. | El evento del intervalo queda asociado al nombre de la ruta. Un evento fuera del intervalo permanece sin ruta. |

## Resultados observados

| Observación | Resultado | Evidencia |
| --- | --- | --- |
| Captura local | 9 valores visibles entre 55,8 y 83,1 BPM. Mediana de 59,7 BPM. | [Monitor serie](../evidencias/avance-3/monitor-serie.jpg) |
| Referencia personal | 71,5 BPM. Los límites mostrados, 57,2 y 85,8 BPM, coinciden con 71,5 × 0,8 y 71,5 × 1,2. La captura del panel no permite medir cuánto duró la calibración. | [Dashboard con referencia y rango](../evidencias/avance-3/dashboard-calibracion.png) |
| Consulta del historial | El dashboard mostró 59,7 BPM como última lectura guardada a las 11:21:08, con «Sin señal reciente». El historial muestra también eventos con hora y estado. | [Última lectura guardada](../evidencias/avance-3/dashboard-lectura-guardada.png) · [Historial](../evidencias/avance-3/dashboard-contacto.jpg) |
| Cambios de estado y umbral | Aparecen los estados «Sin contacto» y «Esperando pulso fiable». El historial muestra dos ejemplos sobre el rango, 116,2 BPM a las 11:26:52 y 114,5 BPM a las 11:28:00. Los eventos visibles figuran sin ruta. | [Historial de cambios](../evidencias/avance-3/dashboard-contacto.jpg) · [Eventos fuera de rango](../evidencias/avance-3/dashboard-fuera-de-rango.jpg) |
| Continuidad del envío | El monitor muestra varios mensajes `Nube: HTTP -1. Reintentando.` junto con lecturas locales. | [Monitor serie](../evidencias/avance-3/monitor-serie.jpg) |

Los valores describen los extractos visibles de la sesión. La coincidencia de 59,7 BPM entre ambas pantallas no permite emparejar un evento ni calcular latencia. La evidencia del historial tampoco basta para observar la activación física del zumbador o confirmar persistencia tras un reinicio. [Datos transcritos](../datos/avance-3.csv).

### Ensayo posterior de envío al dashboard

El [video de 34 segundos](../evidencias/avance-3/prueba-envio-dashboard.mp4) muestra el monitor serie y el dashboard del reloj de prueba. En el monitor se distinguen respuestas `HTTP 201` para envíos de 104,6, 98,0, 98,9 y 105,5 BPM, entre otros. La tarjeta de último pulso cambia durante la grabación. Esto confirma varios envíos aceptados y una actualización visible en Railway. Para calcular latencia y tasa de entrega todavía se necesita emparejar los eventos por identificador y contabilizar todos los intentos.

## Fallas y seguimiento

**Lectura intermitente del sensor.** Durante una prueba previa de banco, el equipo rehízo las soldaduras y volvió a ejecutar el escáner I²C. El escáner volvió a detectar el MAX30102 y la conexión se estabilizó según el registro del equipo. Las capturas de esta sesión muestran lecturas posteriores de BPM. [Registro en la presentación del equipo](https://docs.google.com/presentation/d/1T4sqJN47vCGOSYmh2CNdfCeFcNfqSjfXCC8BawA_nTc/edit#slide=id.p9).

**Envío intermitente.** En el firmware, el valor `-1` también se usa como valor inicial cuando un intento no llega a obtener una respuesta HTTP. Con el log disponible no se puede atribuir el fallo a una causa concreta. El envío corre en una tarea separada, mantiene temporalmente los datos en una cola en RAM y reintenta con espera creciente. La sesión muestra que la placa siguió estimando BPM mientras reintentaba. Para verificar la recuperación, registrar estado Wi-Fi, resultado de la solicitud e ID del evento en un mismo ensayo, y cotejarlo con el historial después de restablecer internet. [Firmware de nube](../../hardware/cardiaco_nube.ino).

Para medir desempeño quedan pendientes una comparación temporalmente emparejada con una referencia de pulso, una serie de envíos con hora de captura y recepción, y una prueba de interrupción y recuperación. Con esos registros se puede calcular el error absoluto medio en BPM, la latencia de cada evento y la tasa de recepción sobre eventos únicos enviados.

## Relación con el Avance 1

El problema identificado fue la somnolencia en rutas largas de carga. Una desviación persistente del pulso respecto de la referencia personal es la señal que el prototipo propone para llamar la atención del conductor y ayudarlo a decidir si conviene detenerse a descansar. La prueba muestra que el panel puede presentar cambios de pulso con dirección y hora para una revisión posterior. La capacidad de detectar somnolencia, la utilidad de la alerta durante un recorrido y su desempeño frente a movimiento requieren validación con conductores en condiciones controladas. [Problema del Avance 1](../../README.md#problema-del-avance-1).
