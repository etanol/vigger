//
// seleccion.cpp - Métodos para implementar la selección.
//
#include <climits>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glwidget.h"


//
// .: setMarcaVis [public slot] :.
//
// Cuando no queramos que se marquen los visibles debemos asegurarnos de que no
// se pintarán marcados. Por eso hacemos un bucle que los desmarque todos.
//
void GLWidget::setMarcaVis (bool si)
{
    sel_marcavisibles = si;
    if (!si)
        for (int i = 0; i < ani_nrobots; i++)
            if (i != sel_robot)
                ani_probot[i].seleccionado = false;
    updateGL();
}


//
// .: seleccionaRobot [private] :.
//
// Dadas unas coordenadas de pantalla "x" e "y", tenemos que encontrar el robot
// que ha sido escogido y hacer las acciones correspondientes. En este método
// entendemos como robot seleccionado aquel que viene dado por el campo
// "sel_robot" y como robot escogido el que se encuentra en las coordenadas "x"
// e "y". La casuística es la siguiente:
//
//   * En las coordenadas no hay ningún robot: no se hace nada.
//   * No hay ningún robot seleccionado: seleccionamos el que se ha escogido.
//   * Hay un robot seleccionado y el escogido no es el mismo: no se hace nada.
//   * Hay un robot seleccionado y el escogido es el mismo: lo deseleccionamos.
//
// Hay que tener cuidado con las coordenada "y" pues en Qt crecen hacia abajo y
// en OpenGL crecen hacia arriba (ver línea marcada con [*]).
//
void GLWidget::seleccionaRobot (int x, int y)
{
    int hits, escogido;
    unsigned int zmin;
    GLuint sbuffer[MAX_ROBOTS * 4];
    GLint  viewport[4];

    // Pasamos a modo selección
    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(MAX_ROBOTS * 4, sbuffer);
    (void) glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);

    // Definimos sólo la perspectiva con el volumen de visualización (pick
    // matrix) y aprovechamos que la modelview ya tiene la cámara situada (sea
    // en primera o en tercera persona)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(x, viewport[3] - y, 3.0, 3.0, viewport); // [*]
    setPerspectiva();
    glMatrixMode(GL_MODELVIEW);

    // Pintamos los robots salvo el que hace de primera persona si la cámara
    // está en tal estado
    for (int i = 0; i < ani_nrobots; i++)
        if ((!cam_primera) || (i != sel_robot)) {
            glLoadName(i);
            glPushMatrix();
            glTranslated(ani_probot[i].x, ani_arobots, ani_probot[i].z);
            glRotatef(ani_probot[i].angulo, 0.0, 1.0, 0.0);
            robot.pinta(false, false); // Sin color ni normales
            glPopMatrix();
        }

    // Restauramos la matriz de proyección
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Procesamos los hits: tenemos que encontrar el hit que tenga la mínima Z
    // mínima (valga la redundancia), es decir, el más próximo al observador;
    // luego decidiremos qué hacer
    hits     = glRenderMode(GL_RENDER) * 4;
    escogido = -1;
    zmin     = UINT_MAX;
    for (int i = 0; i < hits; i += 4)
        if (sbuffer[i+1] < zmin) {
            zmin     = sbuffer[i+1];
            escogido = static_cast<int>(sbuffer[i+3]);
        }

    // Ningún robot escogido, nada que hacer
    if (escogido == -1)
        return;

    if (sel_robot >= 0) {
        // El robot escogido es diferente al seleccionado, nada tampoco
        if (escogido != sel_robot)
            return;
        // El robot escogido es el robot seleccionado, deseleccionamos y
        // desmarcamos todos los visibles si es necesario
        if (sel_marcavisibles)
            for (int i = 0; i < ani_nrobots; i++)
                ani_probot[i].seleccionado = false;
        else
            ani_probot[escogido].seleccionado = false;
        sel_robot = -1;
    } else {
        // El robot escogido pasa a ser el robot seleccionado
        ani_probot[escogido].seleccionado   = true;
        ani_probot[escogido].tipo_seleccion = 0;
        sel_robot = escogido;
    }
    emit nuevaSel(sel_robot);
    updateGL();
}


//
// .: marcaVisibles [private] :.
//
// Éste método algo más sencillo que el anterior porque se trata de un recorrido
// y no una búsqueda. Además no necesitamos el pick matrix, con lo que no es
// necesario consultar el viewport ni las coordenadas del ratón. Únicamente hace
// falta poner la cámara en primera persona, pintar los robots en modo selección
// (salvo el robot que hace de cámara) y ver cuántos hits hay para marcar los
// correspondientes afectados.
//
// La activación de esta función carga mucho la aplicación porque prácticamente
// pinta dos veces la escena en cada frame. Por eso comprobamos si no hay ningún
// robot seleccionado justo al principiom, para ver si nos podemos ahorrar
// trabajo.
//
void GLWidget::marcaVisibles()
{
    if (sel_robot < 0) return;

    int    hits;
    GLuint sbuffer[MAX_ROBOTS * 4];

    // Pasamos a modo selección
    glSelectBuffer(MAX_ROBOTS * 4, sbuffer);
    (void) glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);

    // Colocamos la cámara en primera persona, lo forzamos en los métodos que
    // modifican la cámara
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    setPerspectiva(true);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    situaCamara(true);

    // Pintamos todos salvo el seleccionado; también aprovechamos este bucle
    // para desmarcarlos, luego se volverán a marcar con los hits
    for (int i = 0; i < ani_nrobots; i++)
        if (i != sel_robot) {
            glLoadName(i);
            glPushMatrix();
            glTranslated(ani_probot[i].x, ani_arobots, ani_probot[i].z);
            glRotatef(ani_probot[i].angulo, 0.0, 1.0, 0.0);
            robot.pinta(false, false); // No hacen falta ni colores ni normales 
            glPopMatrix();
            ani_probot[i].seleccionado = false;
        }

    // Restauramos la matriz de proyección; la de modelización no es necesario
    // restuararla porque "paintGL()" ya la pone como toca
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Procesamos los hits y marcamos lo que haga falta
    hits = glRenderMode(GL_RENDER) * 4;
    for (int i = 0; i < hits; i += 4) {
        ani_probot[sbuffer[i+3]].seleccionado   = true;
        ani_probot[sbuffer[i+3]].tipo_seleccion = 1;
    }
}

