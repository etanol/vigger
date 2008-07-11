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

#endif
