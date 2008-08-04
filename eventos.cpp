//
// eventos.cpp - Gestión de eventos del GLWidget.
//
#include "glwidget.h"
#include <QKeyEvent>


//
// .: keyPressEvent [protected virtual] :.
//
// Gestionamos pocos eventos de teclado porque sino necesitaríamos muchas
// señales para que el control desde el teclado y el control desde la interfaz
// estuvieran sincronizados.
//
void GLWidget::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
        case Qt::Key_A:
            cam_anguloa0 += 5;
            ajustaApertura();
            break;

        case Qt::Key_Z:
            cam_anguloa0 -= 5;
            ajustaApertura();
            break;

        case Qt::Key_Plus:
            cam_distancia -= cam_avance;
            emit nuevaDist(entero(cam_distancia));
            ajustaRecorteZ();
            break;

        case Qt::Key_Minus:
            cam_distancia += cam_avance;
            emit nuevaDist(entero(cam_distancia));
            ajustaRecorteZ();
            break;

        case Qt::Key_U:
            cam_anguloz -= 5;
            if (cam_anguloz < 0)
                cam_anguloz += 360;
            emit nuevoAnguloZ(cam_anguloz);
            break;

        case Qt::Key_O:
            cam_anguloz += 5;
            if (cam_anguloz > 360)
                cam_anguloz -= 360;
            emit nuevoAnguloZ(cam_anguloz);
            break;

        case Qt::Key_R:
            reseteaCam();
            break;

        default:
            ev->ignore();
            return;
    }
    updateGL();
    ev->accept();
}


//
// .: mousePressEvent [protected virtual] :.
//
// Según el botón que es pulse rotaremos la escena o la desplazaremos (panning).
// Nos guardamos las coordenadas iniciales (y otros estados de la cámara) para
// poder comparar más adelante.
//
// También se puede seleccionar cualquier robot si pulsamos la tecla "Shift" y
// el botón izquierdo del ratón a la vez.
//
void GLWidget::mousePressEvent (QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        // Guardamos los ángulos de la cámara, pero sólo los necesarios
        aux_angulox = cam_angulox;
        aux_anguloy = cam_anguloy;
        if (ev->modifiers() == Qt::ShiftModifier)
            seleccionaRobot(ev->x(), ev->y());
    }
    else if (ev->button() == Qt::RightButton)
    {
        // Guardamos el estado del panning
        aux_panx = cam_panx;
        aux_pany = cam_pany;
    }
    // Evidentemente, nos guardamos las coordenadas del ratón para poder
    // calcular las variaciones de movimiento
    aux_rx = ev->x();
    aux_ry = ev->y();
    ev->accept();
}


//
// .: mouseMoveEvent [protected virtual] :.
//
// Como deshabilitamos el tracking del ratón estos eventos sólo se producirán
// con un arrastre, es decir, movimientos del ratón manteniendo algún botón
// pulsado.
//
void GLWidget::mouseMoveEvent (QMouseEvent *ev)
{
    if (ev->buttons() & Qt::LeftButton)
    {
        // Restauramos los ángulos guardados y sumamos las variaciones; luego
        // rectificamos los ángulos
        cam_anguloy = aux_anguloy + (ev->x() - aux_rx);
        cam_angulox = aux_angulox + (ev->y() - aux_ry);
        rectificaRotaciones(); // Los emits se hacen aquí
    }
    else if (ev->buttons() & Qt::RightButton)
    {
        // Restauramos el panning y sumamos las variaciones.  Los casts se hacen
        // implícitamente
        cam_panx = aux_panx + (cam_avance * (ev->x() - aux_rx));
        cam_pany = aux_pany + (cam_avance * (aux_ry  - ev->y()));
        emit nuevoPanX(cam_panx);
        emit nuevoPanY(cam_pany);
    }
    ev->accept();
    updateGL();
}


//
// .: wheelEvent [protected virtual] :.
//
// Para explotar al máximo el dispositivo del ratón se decidió aprovechar este
// evento para aquellos ratones que tuvieran la rueda del scroll.  Así se
// implementan los zooms: óptico y de proximidad.
//
void GLWidget::wheelEvent (QWheelEvent *ev)
{
    if (ev->modifiers() == Qt::ControlModifier)
    {
        cam_anguloa0 += signo(ev->delta()) * 4;
        ajustaApertura();
    }
    else
    {
        cam_distancia += (signo(ev->delta())*16) * cam_avance;
        emit nuevaDist(entero(cam_distancia));
        ajustaRecorteZ();
    }
    ev->accept();
    updateGL();
}

