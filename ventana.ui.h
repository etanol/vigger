//
// ventana.ui.h - Extensión de código de la clase Ventana.
//
#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qcolordialog.h>
#include <qcolor.h>
#include "ayuda.h"


//
// init - Inicialización de la ventana.
//
void Ventana::init ()
{
    // Configuramos la ventana de selección de modelos
    seleccionModelo = new QFileDialog(this, 0, TRUE);
    ventanaAyuda    = new Ayuda(this);
    seleccionModelo->setFilter("Models 3DS (*.3ds)");

    // Estos valores son 'hardcoded', así que cuidado al cambiarlos; consultar
    // glfuncs.cpp
    btnFrustum->setRGB(0, 0, 200);
    btnSeleccionado->setRGB(240, 240, 0);
    btnVisibles->setRGB(0, 240, 0);
    btnColorFondo->setRGB(102, 102, 102);
    cambiaLuz(0);
}


//
// cargaAyuda - Muestra la ventana de ayuda.
//
void Ventana::cargaAyuda ()
{
    ventanaAyuda->show();
}


//
// cargaEscenario - Muestra un diálogo para seleccionar un fichero que se carga
//                  como escenario.
//
void Ventana::cargaEscenario ()
{
    if (seleccionModelo->exec() == QDialog::Accepted) {
        if (glVista->cargaEscenario(seleccionModelo->selectedFile())) {
            // Ya podemos cargar robots
            btnCargaRo->setEnabled(true);
        } else {
            // Imprime error
            QFileInfo fi(seleccionModelo->selectedFile());
            QMessageBox::critical(this, "Error",
                    "No se ha podido abrir el fichero " + fi.fileName(),
                    QMessageBox::Ok, QMessageBox::NoButton);
        }
    }
}


//
// cargaRobot - Muestra un diálogo para seleccionar un fichero que se cargará
//              como robot.
//
void Ventana::cargaRobot ()
{
    if (seleccionModelo->exec() == QDialog::Accepted) {
        if (!glVista->cargaRobots(seleccionModelo->selectedFile())) {
            // Error
            QFileInfo fi(seleccionModelo->selectedFile());
            QMessageBox::critical(this, "Error",
                    "No se ha podido abrir el fichero " + fi.fileName(),
                    QMessageBox::Ok, QMessageBox::NoButton);
        }
    }
}


//
// actualizaRadio - Ajusta los sliders necesarios según el valor de "radio".
//
void Ventana::actualizaRadio (double radio)
{
    sldDistancia->setMaxValue(static_cast<int>(100.0*radio));
    sldDistancia->setMinValue(0);
    sldZFarPrimera->setMaxValue(static_cast<int>(20.0*radio));
    sldZFarPrimera->setValue(static_cast<int>(20.0*radio));
}


//
// actualizaAlguras - Ajusta el slider de altura de los robots teniendo en
//                    cuenta "maxescenaY" y "maxrobotY".
//
void Ventana::actualizaAlturas (int maxescenaY, int maxrobotY)
{
    sldAltura->setMinValue(maxrobotY - maxescenaY);
    sldAltura->setMaxValue(maxescenaY - maxrobotY);
    sldAltura->setValue(maxrobotY);
}


//
// setColorLuz - Cambia alguna componente de la luz actual seleccionada en la
//               ventana.
//
void Ventana::setColorLuz (VPushButton *b, int componente)
{
    QColor c;

    c = QColorDialog::getColor(QColor(b->getR(), b->getG(), b->getB()), this);
    b->setRGB(c.red(), c.green(), c.blue());
    glVista->setColLuz(cmbLuz->currentItem(), componente, c.red(), c.green(),
            c.blue());
    glVista->updateGL();
}
// Ahora enlazamos con los clicks de los botones
void Ventana::setColorAmbiente  () { setColorLuz(btnAmbiente,  0); }
void Ventana::setColorDifusa    () { setColorLuz(btnDifusa,    1); }
void Ventana::setColorEspecular () { setColorLuz(btnEspecular, 2); }


//
// coordenadasLuz - Cambia las coordenadas y la referencia de la luz actual
//                  seleccionada en la ventana.
//
void Ventana::coordenadasLuz ()
{
    bool  okx, oky, okz;
    float valx, valy, valz;

    valx = txtlCoordX->text().toFloat(&okx);
    valy = txtlCoordY->text().toFloat(&oky);
    valz = txtlCoordZ->text().toFloat(&okz);
    if (okx && oky && okz) {
        glVista->setPosLuz(cmbLuz->currentItem(), valx, valy, valz,
                cmbReferencia->currentItem() == 0);
        glVista->updateGL();
    } else {
        QMessageBox::critical(this, "Error",
                "Coordenadas incorrectas", QMessageBox::Ok,
                QMessageBox::NoButton);
    }
}


//
// colorSeleccionados - Selecciona el color de los seleccionados (valga la
//                      redundancia) según el botón que se haya pulsado.
//
void Ventana::colorSeleccionados (VPushButton *b, int indice)
{
    QColor c;

    c = QColorDialog::getColor(QColor(b->getR(), b->getG(), b->getB()), this);
    b->setRGB(c.red(), c.green(), c.blue());
    glVista->setColSelec(indice, c.red(), c.green(), c.blue());
    glVista->updateGL();
}
// Ahora enlazamos con los clicks de los botones
void Ventana::setColorSeleccionado () { colorSeleccionados(btnSeleccionado,0); }
void Ventana::setColorVisibles     () { colorSeleccionados(btnVisibles,    1); }


//
// colorFrustum - Cambia el color del frustum y ya está.
//
void Ventana::colorFrustum ()
{
    QColor c;

    c = QColorDialog::getColor(QColor(btnFrustum->getR(), btnFrustum->getG(),
                btnFrustum->getB()), this);
    btnFrustum->setRGB(c.red(), c.green(), c.blue());
    glVista->setColFrust(c.red(), c.green(), c.blue());
    glVista->updateGL();
}


//
// colorFondo - Cambia el color de fondo de la escena.
//
void Ventana::colorFondo ()
{
    QColor c;

    c = QColorDialog::getColor(QColor(btnColorFondo->getR(),
                btnColorFondo->getG(), btnColorFondo->getB()), this);
    btnColorFondo->setRGB(c.red(), c.green(), c.blue());
    glVista->setColorFondo(c.red(), c.green(), c.blue());
    glVista->updateGL();
}


//
// cambiaLuz - Cambia la luz seleccionada y actualiza la parte de luz en la
//             interfaz. Uff, qué palo.
//
void Ventana::cambiaLuz (int luz)
{
    int   rgb[3];
    float *pos;
    bool enable = glVista->luzActivada(luz);

    // Replicamos el código de la función "cambioEstadoLuz()" porque aquí
    // necesitamos modificar el estado de "chkLActiva" sin que se emita ninguna
    // señal
    chkLActiva->blockSignals(true);
    chkLActiva->setChecked(enable);
    btnAmbiente->setEnabled(enable);
    btnDifusa->setEnabled(enable);
    btnEspecular->setEnabled(enable);
    txtlCoordX->setEnabled(enable);
    txtlCoordY->setEnabled(enable);
    txtlCoordZ->setEnabled(enable);
    cmbReferencia->setEnabled(enable);
    chkLActiva->blockSignals(false);

    // Consultamos los colores
    glVista->getColLuz(luz, 0, rgb);
    btnAmbiente->setRGB(rgb[0], rgb[1], rgb[2]);
    glVista->getColLuz(luz, 1, rgb);
    btnDifusa->setRGB(rgb[0], rgb[1], rgb[2]);
    glVista->getColLuz(luz, 2, rgb);
    btnEspecular->setRGB(rgb[0], rgb[1], rgb[2]);

    // I les coordenades
    cmbReferencia->setCurrentItem(glVista->getRefLuz(luz) ? 0 : 1);
    pos = glVista->getPosLuz(luz);
    txtlCoordX->setText(QString::number(pos[0], 'f', 3));
    txtlCoordY->setText(QString::number(pos[1], 'f', 3));
    txtlCoordZ->setText(QString::number(pos[2], 'f', 3));
}


//
// cambiaEstadoLuz - Activa o desactiva la luz actual.
//
void Ventana::cambiaEstadoLuz (bool estado)
{
    glVista->activaLuz(cmbLuz->currentItem(), estado);

    // Activamos y desactivamos según el booleano
    btnAmbiente->setEnabled(estado);
    btnDifusa->setEnabled(estado);
    btnEspecular->setEnabled(estado);
    txtlCoordX->setEnabled(estado);
    txtlCoordY->setEnabled(estado);
    txtlCoordZ->setEnabled(estado);
    cmbReferencia->setEnabled(estado);

    // Actualizamos la posició por si acaso
    coordenadasLuz();
}

