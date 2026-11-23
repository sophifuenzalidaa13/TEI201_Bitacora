# Hardware de Pulso Ruta

- [Lista de materiales, justificación y costos](bom/README.md)
- [Cableado de señal y simulación Wokwi](esquemas/README.md)
- [Fotografías del prototipo ensamblado](fotos/README.md)
- [Diseño del encapsulado](../diseno_3d/README.md)

## Montaje de banco

1. Con la alimentación desconectada, monta el MAX30102 y el zumbador con la masa común indicada en el [esquema](esquemas/README.md).
2. Comprueba que los pines de SDA, SCL y zumbador coincidan con los que configuraste en el sketch.
3. Conecta la placa por USB y abre el monitor serie a 115200 baudios. Comprueba que el sensor se detecte antes de iniciar la calibración.
4. Sigue el [protocolo de pruebas](../testing/reportes/avance-3.md) para verificar lecturas, estados y envío.

La tapa separada del encapsulado está diseñada para dar acceso a la placa y a la batería. Durante la demo queda por mostrar la apertura y el acceso a los componentes del montaje real. El circuito de alimentación portátil, interruptor y carga queda por documentar.
