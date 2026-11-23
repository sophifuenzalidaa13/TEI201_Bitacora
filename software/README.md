# Software de Pulso Ruta

## Firmware y panel

| Versión | Código e instalación | Función |
| --- | --- | --- |
| Panel local | [Sketch Arduino](src/Monitor_MAX30102_WiFi_KY006_Activo.ino) · [Manual de instalación](docs/INSTALACION.md) | El ESP32-S3 crea su propia red Wi-Fi y sirve el panel en el teléfono. |
| Dashboard en la nube | [Sketch Arduino](../hardware/cardiaco_nube.ino) · [Instrucciones de carga](../README.md#dashboard-publicado-en-railway) | El ESP32-S3 envía lecturas y cambios de estado por HTTPS a un servidor Express en Railway. PostgreSQL en Aiven conserva el historial por reloj. |

El [dashboard publicado](https://pulso-ruta-production.up.railway.app/) muestra el pulso en BPM, la referencia personal, la banda de ±20 % y las fluctuaciones. El código del servidor y de la interfaz se mantiene en un repositorio de despliegue privado. El equipo administra el acceso al código y las cuentas para evaluar el panel.

## Captura y procesamiento en la versión de nube

El firmware publicado configura el MAX30102 a 400 conversiones por segundo y promedia cuatro conversiones por muestra en el FIFO, lo que deja 100 muestras por segundo. Un intervalo de 10 ms entre muestras permite seguir el pulso óptico y distinguir latidos separados por cientos de milisegundos. El programa estima los BPM a partir del intervalo entre latidos y, después de calibrar, promedia cuatro estimaciones válidas. El zumbador se activa tras tres estimaciones procesadas consecutivas fuera de la banda de referencia.

Cada cambio de estado se encola para envío. En condiciones normales también se encola una actualización cada cinco segundos aproximadamente. Esa cadencia permite consultar el historial sin enviar cada muestra óptica. La tasa de captura y el intervalo de actualización son parámetros del firmware publicado, y las capturas del Avance 3 provienen de una versión de prueba anterior.

- [Arquitectura del panel local](docs/ARQUITECTURA.md)
- [Librerías, código externo y uso de IA](../FUENTES.md)
- [Resultados del ensayo de banco](../testing/reportes/avance-3.md)
