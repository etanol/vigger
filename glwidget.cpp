//
// glwidget.cpp - Constructor y otros métodos no clasificados.
//
#include <qdatetime.h>
#include <cstdlib>
#include "glwidget.h"


/////////////////////
//                 //
//   CONSTRUCTOR   //
//                 //
/////////////////////
GLWidget::GLWidget (QWidget *parent, const char *name)
        : QGLWidget(parent, name)
{
    // Inicializamos la raíz aleatoria
    QTime medianoche(0, 0, 0);
    srandom(medianoche.secsTo(QTime::currentTime()));

    setMouseTracking(FALSE);

    // Valores por defecto
    ani_nrobots   = 0;
    ani_timer     = 0;
    ani_tinterval = 100;
    ani_arobots   = 0.0;
    ani_avance    = 0.0;
    ani_vangular  = 10;
    ani_vlineal   = 1;

    sel_robot         = -1;
    sel_marcavisibles = false;
    sel_pintafrustum  = false;

    cam_zfarpri = 0.0; // Este sólo se actualiza desde el slot
    reseteaCam();
}


//
// .: cargaEscenario [public] :.
//
bool GLWidget::cargaEscenario (const char *fichero)
{
    if (!escenario.carga(fichero))
        return false;

    cam_avance = escenario.radio() * 0.01;
    robot.redimensiona(0.1 * escenario.radio());
    ajustaVLineal();
    ani_arobots = robot.maxY();

    emit alturas(entero(escenario.maxY()), entero(robot.maxY()));
    emit nuevoRadio(escenario.radio());
    emit nuevoRadioXZ(escenario.radioXZ());

    // Reseteamos la cámara después de emitir los signals para que el slider de
    // la distancia se actualice bien
    reseteaCam();

    // El tamaño y posición de los robots dependen de las características del
    // escenario; así que tenemos que reinicializarlos todos
    for (int i = 0; i < ani_nrobots; i++)
        nuevoRobot(i);
    // Así mismo, si había algún robot seleccionado lo debemos restaurar
    if (sel_robot >= 0) {
        ani_probot[sel_robot].seleccionado   = true;
        ani_probot[sel_robot].tipo_seleccion = 0;
    }
    updateGL();
    return true;
}


//
// .: cargaRobots [public] :.
//
bool GLWidget::cargaRobots (const char *fichero)
{
    if (!robot.carga(fichero))
        return false;

    robot.redimensiona(0.1 * escenario.radio());
    ajustaVLineal();
    ani_arobots = robot.maxY();

    emit alturas(entero(escenario.maxY()), entero(robot.maxY()));

    // El cambio en el tamaño de los robots puede provocar colisiones
    for (int i = 0; i < ani_nrobots; i++)
        nuevoRobot(i);
    // Igual que antes, cuidado con el seleccionado
    if (sel_robot >= 0) {
        ani_probot[sel_robot].seleccionado   = true;
        ani_probot[sel_robot].tipo_seleccion = 0;
    }
    updateGL();
    return true;
}

