//
// ventana.h - Conexión del formulario ventana.ui con el resto del código.
//
// En Qt4, los formularios diseñados con QtDesigner ya no hacen uso de los
// ficheros conextensión .ui.h que servían para incorporar código escrito
// manualmente al código generado por el comando uic.
//
// Ahora, el QtDesigner únicamente crea formularios .ui y es responsabilidad del
// programador crear una clase adicional para incorporar el código generado por
// el uic de Qt4.
//
#ifndef _VENTANA_H_
#define _VENTANA_H_

#include <QWidget>
#include "ui_ventana.h"

class QFileDialog;
class Ayuda;

class Ventana : public QWidget, private Ui::Ventana
{
    Q_OBJECT

public:

    Ventana ();

private slots:

    void cargaAyuda           ();
    void cargaEscenario       ();
    void cargaRobot           ();
    void actualizaRadio       (double radi );
    void actualizaAlturas     (int maxescenaY, int maxrobotY);
    void setColorAmbiente     ();
    void setColorDifusa       ();
    void setColorEspecular    ();
    void coordenadasLuz       ();
    void setColorSeleccionado ();
    void setColorVisibles     ();
    void colorFrustum         ();
    void colorFondo           ();
    void cambiaLuz            (int luz);
    void cambiaEstadoLuz      (bool estado);

private:

    QFileDialog *seleccionModelo;
    Ayuda       *ventanaAyuda;

    void setColorLuz        (VPushButton * b, int componente);
    void colorSeleccionados (VPushButton * b, int indice);

};

#endif //_VENTANA_H_
