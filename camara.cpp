//
// camara.cpp - Funciones para ajustar la cámara.
//
#include <GL/gl.h>
#include <GL/glu.h>
#include "glwidget.h"


//
// .: reseteaCam [public slot] :.
//
// Ponemos la camara de 3a persona en el estado inicial.
//
void GLWidget::reseteaCam ()
{
    cam_angulox   = 0;
    cam_anguloy   = 0;
    cam_anguloz   = 0;
    cam_panx      = 0.0;
    cam_pany      = 0.0;
    cam_distancia = 2.0 * escenario.radio();
    cam_anguloa0  = 60;
    cam_primera   = false;

    ajustaRecorteZ();
    ajustaApertura();

    emit nuevoAnguloX(cam_angulox);
    emit nuevoAnguloY(cam_anguloy);
    emit nuevoAnguloZ(cam_anguloz);
    emit nuevoPanX(cam_panx);
    emit nuevoPanY(cam_pany);
    emit nuevaDist(entero(cam_distancia));
    updateGL();
}


//
// .: setCamPrimera [public slot] :.
//
// Activa o desactiva la cámara en primera persona.  Como la matriz de
// proyección no se actualiza en cada frame lo tenemos que hacer ahora.  Si no
// hay ningún robot seleccionado ignoramos la solicitud del usuario.
//
void GLWidget::setCamPrimera (bool si)
{
    cam_primera = si && (sel_robot >= 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspectiva();
    glMatrixMode(GL_MODELVIEW);
    updateGL();
}


//
// .: setZFarCamPri [public slot] :.
//
// Cambiamos el plano de recorte Z-Far de los robots.  Así cuando visualizamos
// el frustum se verá más pequeño y cuando tengamos la cámara en primera persona
// el recorte se notará más.  Si estamos en modo primera persona se debe
// actualizar la matriz de proyección.
//
void GLWidget::setZFarCamPri (int z)
{
    cam_zfarpri = decimal(z);
    if (cam_primera) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        setPerspectiva();
        glMatrixMode(GL_MODELVIEW);
    }
    updateGL();
}


//
// .: setDistancia [public slot] :.
//
// Ajustamos la diestancia del observador a la escena.  Este slot se sencillo
// dado que todo el trabajo importante lo hace "ajustaRecorteZ()".
//
void GLWidget::setDistancia (double dist)
{
    cam_distancia = dist;
    emit nuevaDist(entero(dist));
    ajustaRecorteZ(); // Este ya hace el "emit"
    updateGL();
}


//
// .: situaCamara [private] :.
//
// Coloca la cámara en su lugar, según el estado actual y el tipo de cámara
// seleccionada.  Con el parámetro "forzar_prim" a verdadero obligamos a colocar
// la cámara en primera persona dependiendo del robot seleccionado, e ignorando
// el estado de "cam_primera".  Aquí se asume que "sel_robot" > -1
//
void GLWidget::situaCamara (bool forzar_prim)
{
    if (cam_primera || forzar_prim) {
        glRotatef(static_cast<GLfloat>(ani_probot[sel_robot].angulo + 180),
                                                                0.0, -1.0, 0.0);
        glTranslated(-ani_probot[sel_robot].x, -ani_arobots,
                                                      -ani_probot[sel_robot].z);
    } else {
        glTranslated(cam_panx, cam_pany, -cam_distancia);
        glRotatef(static_cast<GLfloat>(cam_anguloz), 0.0, 0.0, -1.0);
        glRotatef(static_cast<GLfloat>(cam_angulox), 1.0, 0.0,  0.0);
        glRotatef(static_cast<GLfloat>(cam_anguloy), 0.0, 1.0,  0.0);
    }
}


//
// .: setPerspectiva [private] :.
//
// Igual que en "situaCamara()", aquí también debemos distingir entre diferentes
// tipos de cámaras.  El parámetro "forzar_prim" tiene el mismo significado.
// Aquí no es necesario que haya algún robot seleccionado; aunque no es
// recomendable dejar de comrobarlo antes.
//
void GLWidget::setPerspectiva (bool forzar_prim)
{
    if (cam_primera || forzar_prim)
        gluPerspective(60.0, cam_aspecto, 0.1, cam_zfarpri);
    else
        gluPerspective(cam_anguloa, cam_aspecto, cam_znear, cam_zfar);
}


//
// .: ajustaApertura [private] :.
//
// Rectifica el ángulo de apertura vertical de la cámara, original o teórico,
// asegurando que pertenezca al intervalo [1, 179].
//
void GLWidget::ajustaApertura ()
{
    if (cam_anguloa0 < 1)
        cam_anguloa0 = 1;
    else if (cam_anguloa0 > 179)
        cam_anguloa0 = 179;

    actualizaAspecto();
    emit nuevoAnguloA(cam_anguloa0);
}


//
// .: actualizaAspecto [private] :.
//
// Para poder ver toda la escena, al menos en el estado inicial de la cámara, se
// debe rectificar el ángulo de apertura vertical cuando la relación de aspecto
// es inferior a 1.  Cuando esto sucede hay que aumentar este ángulo sin alterar
// la relación de aspecto.  En la implementación es difícil ver la fórmula:
//
//              / tan O  \
//     W = atan | ------ |
//              \   AR   /
//
// Donde "O" es la mitad del ángulo de apertura original, "W" es la mitad del
// ángulo de apertura corregido y "AR" es la relación de aspecto.
//
void GLWidget::actualizaAspecto ()
{
    if (cam_aspecto < 1.0)
        cam_anguloa = 2.0 * grados(atan(tan(radianes(
                      static_cast<double>(cam_anguloa0) / 2.0)) / cam_aspecto));
    else
        cam_anguloa = static_cast<double>(cam_anguloa0);

    // Actualizamos los cambios
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspectiva();
    glMatrixMode(GL_MODELVIEW);
}


//
// .: ajustaRecorteZ [private] :.
//
// Para aprovechar mejor los bits del Z-buffer calculamos los planos de recorte
// frontal y posterior lo más ajustados posible a la escenak sin perder partes
// visibles.  Por suerte, sabemos que el escenario tiene una dimensión limitada
// y que ningún robot saldrá de este volumen.
//
void GLWidget::ajustaRecorteZ ()
{
    cam_znear = cam_distancia - escenario.radio();
    cam_zfar  = cam_znear + escenario.radio() * 2.0;
    if (cam_znear < 0.1) {
        cam_znear = 0.1;
        cam_zfar  = 2.0 * escenario.radio();
    }

    emit nuevoZNear(cam_znear);
    emit nuevoZFar(cam_zfar);

    // Actualizamos los cambios
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspectiva();
    glMatrixMode(GL_MODELVIEW);
}


//
// .: rectificaRotaciones [private] :.
//
// A pesar de no ser necesario para OpenGL que los ángulos de rotación caigan en
// el intervalo [0, 360) resulta práctico de cara a simplificar la conexión de
// señales con los slots de la interfaz.
//
void GLWidget::rectificaRotaciones ()
{
    cam_angulox = cam_angulox % 360;
    cam_anguloy = cam_anguloy % 360;

    if (cam_angulox < 0)  cam_angulox += 360;
    if (cam_anguloy < 0)  cam_anguloy += 360;

    emit nuevoAnguloX(cam_angulox);
    emit nuevoAnguloY(cam_anguloy);
}

