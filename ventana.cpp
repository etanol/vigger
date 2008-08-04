//
// ventana.cpp - Implementación de la clase Ventana
//
// Este fichero reemplaza el antiguo ventana.ui.h siguiendo los cambios
// determinados por la migración de Qt3 a Qt4.
//
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QColor>

#include "ventana.h"
#include "ayuda.h"


Ventana::Ventana () : QWidget(0)
{
    setupUi(this);

    // Conexiones de signals y slots.  Para verlo mejor, ampliar la anchura de
    // la ventana del editor hasta 110 columnas.
    //
    // Las conexiones se programan aquí ya que en el QtDesigner de Qt4 no
    // permite utilizar signals y slots definidos por el usuario.
    connect(btnAyuda,         SIGNAL(clicked()),            this,            SLOT(cargaAyuda()));
    connect(btnAmbiente,      SIGNAL(clicked()),            this,            SLOT(setColorAmbiente()));
    connect(btnCargaEs,       SIGNAL(clicked()),            this,            SLOT(cargaEscenario()));
    connect(btnCargaRo,       SIGNAL(clicked()),            this,            SLOT(cargaRobot()));
    connect(btnColorFondo,    SIGNAL(clicked()),            this,            SLOT(colorFondo()));
    connect(btnDifusa,        SIGNAL(clicked()),            this,            SLOT(setColorDifusa()));
    connect(btnEspecular,     SIGNAL(clicked()),            this,            SLOT(setColorEspecular()));
    connect(btnFrustum,       SIGNAL(clicked()),            this,            SLOT(colorFrustum()));
    connect(btnResetCam,      SIGNAL(clicked()),            glVista,         SLOT(reseteaCam()));
    connect(btnSeleccionado,  SIGNAL(clicked()),            this,            SLOT(setColorSeleccionado()));
    connect(btnSalir,         SIGNAL(clicked()),            this,            SLOT(close()));
    connect(btnVisibles,      SIGNAL(clicked()),            this,            SLOT(setColorVisibles()));
    connect(chkCamPrimera,    SIGNAL(toggled(bool)),        glVista,         SLOT(setCamPrimera(bool)));
    connect(chkAlambre,       SIGNAL(toggled(bool)),        glVista,         SLOT(pintaAlambre(bool)));
    connect(chkLActiva,       SIGNAL(toggled(bool)),        this,            SLOT(cambiaEstadoLuz(bool)));
    connect(chkMarcVisibles,  SIGNAL(toggled(bool)),        glVista,         SLOT(setMarcaVis(bool)));
    connect(chkMovimiento,    SIGNAL(toggled(bool)),        glVista,         SLOT(animaRobots(bool)));
    connect(chkVisFrustum,    SIGNAL(toggled(bool)),        glVista,         SLOT(setPintaFrus(bool)));
    connect(cmbLuz,           SIGNAL(activated(int)),       this,            SLOT(cambiaLuz(int)));
    connect(cmbReferencia,    SIGNAL(activated(int)),       this,            SLOT(coordenadasLuz()));
    connect(cmbRotaRobots,    SIGNAL(activated(int)),       glVista,         SLOT(setRotacionRob(int)));
    connect(glVista,          SIGNAL(nuevoAR(double)),      dspAR,           SLOT(display(double)));
    connect(glVista,          SIGNAL(nuevoPanX(double)),    dspPanX,         SLOT(display(double)));
    connect(glVista,          SIGNAL(nuevoPanY(double)),    dspPanY,         SLOT(display(double)));
    connect(glVista,          SIGNAL(nuevoZNear(double)),   dspzNear,        SLOT(display(double)));
    connect(glVista,          SIGNAL(nuevoZFar(double)),    dspzFar,         SLOT(display(double)));
    connect(glVista,          SIGNAL(nuevoAnguloA(int)),    dspApertura,     SLOT(display(int)));
    connect(glVista,          SIGNAL(nuevoAnguloX(int)),    sldAnguloX,      SLOT(setValue(int)));
    connect(glVista,          SIGNAL(nuevoAnguloY(int)),    sldAnguloY,      SLOT(setValue(int)));
    connect(glVista,          SIGNAL(nuevoAnguloZ(int)),    sldAnguloZ,      SLOT(setValue(int)));
    connect(glVista,          SIGNAL(nuevoAnguloA(int)),    sldApertura,     SLOT(setValue(int)));
    connect(glVista,          SIGNAL(nuevaDist(int)),       sldDistancia,    SLOT(setValue(int)));
    connect(glVista,          SIGNAL(nuevaSel(int)),        dspSeleccionado, SLOT(display(int)));
    connect(glVista,          SIGNAL(nuevoRadio(double)),   dspRadio,        SLOT(display(double)));
    connect(glVista,          SIGNAL(nuevoRadioXZ(double)), dspRadioXZ,      SLOT(display(double)));
    connect(glVista,          SIGNAL(nuevoRadio(double)),   this,            SLOT(actualizaRadio(double)));
    connect(glVista,          SIGNAL(alturas(int,int)),     this,            SLOT(actualizaAlturas(int,int)));
    connect(sldAltura,        SIGNAL(sliderMoved(int)),     glVista,         SLOT(setAltura(int)));
    connect(sldAltura,        SIGNAL(valueChanged(int)),    dspAltura,       SLOT(displayFloat(int)));
    connect(sldAnguloX,       SIGNAL(valueChanged(int)),    dspAnguloX,      SLOT(display(int)));
    connect(sldAnguloX,       SIGNAL(sliderMoved(int)),     glVista,         SLOT(setAnguloX(int)));
    connect(sldAnguloY,       SIGNAL(valueChanged(int)),    dspAnguloY,      SLOT(display(int)));
    connect(sldAnguloY,       SIGNAL(sliderMoved(int)),     glVista,         SLOT(setAnguloY(int)));
    connect(sldAnguloZ,       SIGNAL(valueChanged(int)),    dspAnguloZ,      SLOT(display(int)));
    connect(sldAnguloZ,       SIGNAL(sliderMoved(int)),     glVista,         SLOT(setAnguloZ(int)));
    connect(sldDistancia,     SIGNAL(valueChanged(int)),    dspDistancia,    SLOT(displayFloat(int)));
    connect(sldDistancia,     SIGNAL(sliderMoved(int)),     glVista,         SLOT(setDistancia(int)));
    connect(sldApertura,      SIGNAL(sliderMoved(int)),     glVista,         SLOT(setApertura(int)));
    connect(sldTransparencia, SIGNAL(valueChanged(int)),    glVista,         SLOT(setTrFrust(int)));
    connect(sldZFarPrimera,   SIGNAL(valueChanged(int)),    glVista,         SLOT(setZFarCamPri(int)));
    connect(spbFPS,           SIGNAL(valueChanged(int)),    glVista,         SLOT(setFPS(int)));
    connect(spbNumRobots,     SIGNAL(valueChanged(int)),    glVista,         SLOT(setNumRobots(int)));
    connect(spbVangular,      SIGNAL(valueChanged(int)),    glVista,         SLOT(setVAngular(int)));
    connect(spbVlineal,       SIGNAL(valueChanged(int)),    glVista,         SLOT(setVLineal(int)));
    connect(txtlCoordX,       SIGNAL(returnPressed()),      this,            SLOT(coordenadasLuz()));
    connect(txtlCoordY,       SIGNAL(returnPressed()),      this,            SLOT(coordenadasLuz()));
    connect(txtlCoordZ,       SIGNAL(returnPressed()),      this,            SLOT(coordenadasLuz()));

    // Configuramos la ventana de selección de modelos
    seleccionModelo = new QFileDialog(this, Qt::Dialog);
    ventanaAyuda    = new Ayuda(this);
    seleccionModelo->setFilter("Models 3DS (*.3ds)");

    // Estos valores son 'hardcoded', así que cuidado al cambiarlos; consultar
    // glfuncs.cpp
    btnFrustum     ->setRGB(  0,   0, 200);
    btnSeleccionado->setRGB(240, 240,   0);
    btnVisibles    ->setRGB(  0, 240,   0);
    btnColorFondo  ->setRGB(102, 102, 102);
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
    if (seleccionModelo->exec() == QDialog::Accepted)
    {
        if (glVista->cargaEscenario(seleccionModelo->selectedFiles()[0]))
            btnCargaRo->setEnabled(true);  // Ya podemos cargar robots
        else
        {
            // Imprime error
            QFileInfo fi(seleccionModelo->selectedFiles()[0]);
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
    if (seleccionModelo->exec() == QDialog::Accepted)
    {
        if (!glVista->cargaRobots(seleccionModelo->selectedFiles()[0]))
        {
            // Error
            QFileInfo fi(seleccionModelo->selectedFiles()[0]);
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
    sldDistancia->setMaximum(static_cast<int>(100.0*radio));
    sldDistancia->setMinimum(0);
    sldZFarPrimera->setMaximum(static_cast<int>(20.0*radio));
    sldZFarPrimera->setValue(static_cast<int>(20.0*radio));
}


//
// actualizaAlguras - Ajusta el slider de altura de los robots teniendo en
//                    cuenta "maxescenaY" y "maxrobotY".
//
void Ventana::actualizaAlturas (int maxescenaY, int maxrobotY)
{
    sldAltura->setMinimum(maxrobotY - maxescenaY);
    sldAltura->setMaximum(maxescenaY - maxrobotY);
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
    glVista->setColLuz(cmbLuz->currentIndex(), componente, c.red(), c.green(),
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
    if (okx && oky && okz)
    {
        glVista->setPosLuz(cmbLuz->currentIndex(), valx, valy, valz,
                cmbReferencia->currentIndex() == 0);
        glVista->updateGL();
    }
    else
        QMessageBox::critical(this, "Error",
                "Coordenadas incorrectas", QMessageBox::Ok,
                QMessageBox::NoButton);
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
//             interfaz.  Uff, qué palo.
//
void Ventana::cambiaLuz (int luz)
{
    int    rgb[3];
    float *pos;
    bool   enable = glVista->luzActivada(luz);

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
    cmbReferencia->setCurrentIndex(glVista->getRefLuz(luz) ? 0 : 1);
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
    glVista->activaLuz(cmbLuz->currentIndex(), estado);

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

