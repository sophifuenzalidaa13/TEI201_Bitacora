# Plan de pruebas del software

Estado general: **pendiente de ejecución y registro de evidencia real**. No marcar una prueba como aprobada antes de efectuarla.

| ID | Prueba | Procedimiento y resultado esperado | Estado | Evidencia |
|---|---|---|---|---|
| P01 | Compilación | Compilar con Arduino-ESP32 3.x y SparkFun MAX3010x sin errores | Pendiente | Captura del IDE |
| P02 | Detección sensor | Conectar MAX30102 y confirmar `MAX30102 conectado` en monitor serie | Pendiente | Captura serie |
| P03 | Red local | Verificar que aparezca la red definida por `NOMBRE_RED` y permita conectarse sin Internet | Pendiente | Captura teléfono |
| P04 | Interfaz | Abrir IP mostrada por monitor serie y visualizar página | Pendiente | Captura web |
| P05 | Medición BPM | Colocar dedo, mantener señal estable y observar BPM > 0 | Pendiente | Video o captura |
| P06 | Calibración | Mantener dedo estable, verificar 30 s y al menos 15 mediciones válidas antes de fijar base | Pendiente | Capturas serie/web |
| P07 | Alarma | Simular valores o escenario controlado de tres mediciones fuera de ±20 %; observar estado de alerta | Pendiente | Registro de prueba |
| P08 | Zumbador | Verificar circuito y comprobar ciclo 3 s ON / 1 s OFF durante alerta | Pendiente | Video y medición eléctrica |
| P09 | Sonido teléfono | Activar sonido desde página y comprobar pitido, y función silenciar | Pendiente | Video |
| P10 | Recalibrar | Presionar botón y confirmar reinicio de referencia | Pendiente | Capturas |
| P11 | Pérdida de señal | Retirar dedo/desconectar lectura y verificar respuesta de la interfaz | Pendiente | Capturas |

## Registro de ensayo
Completar para cada ejecución:
- Fecha y responsables:
- Hardware usado y conexiones:
- Versión Arduino IDE / ESP32 / SparkFun:
- Prueba ID:
- Procedimiento realizado:
- Resultado observado:
- ¿Cumple?: Sí / No / Parcial
- Fallas, correcciones, enlaces a fotos o videos:

## Criterios importantes
El zumbador debe probarse solo después de verificar el cableado y que la salida GPIO no esté sobrecargada. La velocidad de pulso, el umbral IR y las alarmas son orientativos y no constituyen validación médica.
