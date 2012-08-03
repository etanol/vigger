//
// glwidget.cpp - Implementación de las funciones básicas para utilizar OpenGL
//                desde las librerías Qt.
//
#include <GL/gl.h>
#include <GL/glu.h>
#include "glwidget.h"


//
// .: initializeGL [protected] :.
//
// Inicialización del pipeline de OpenGL.  Los valores iniciales de colores y
// luz no se han parametrizado por falta de tiempo y por pereza.  Cualquier
// cambio en los colores iniciales se debe reflejar en la interfaz también
// (ventana.ui.h).
//
void GLWidget::initializeGL ()
{
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float a[4] = { 0.1, 0.1, 0.1, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, a);

    // Valores OpenGL iniciales en nuestra aplicación
    setColLuz(0, 0,  10,  10,  10); // Ambiente
    setColLuz(0, 1, 255, 255, 255); // Difusa
    setColLuz(0, 2, 100, 100, 100); // Especular
    setPosLuz(0, 0.0, 1.0, 0.0, false);
    glEnable(GL_LIGHT0);

    setColLuz(1, 0,  10,  10,  10); // Ambiente
    setColLuz(1, 1, 255, 255, 255); // Difusa
    setColLuz(1, 2, 100, 100, 100); // Especular
    setPosLuz(1, 0.0, 0.0, 0.0, true);
    glEnable(GL_LIGHT1);

    // Copia literal del método "setTrFrust()" porque no podemos llamar a
    // "updateGL()" aquí pues se produciría una recursión infinita:
    //     updateGL => initializeGL => updateGL => initializeGL => ...
    col_frust_amb[3] = 0.5;
    col_frust_dif[3] = 0.5;
    col_frust_esp[3] = 0.5;
    setColFrust(0, 0, 200);
    setColSelec(0, 240, 240, 0);
    setColSelec(1, 0, 240, 0);
}


//
// .: resizeGL [protected] :.
//
// Redimensionado del área de pintado de nuestro GLWidget.  Prinipalmente
// interesa calcular la nueva relación de aspecto para aprovechar al máximo el
// nuevo área.
//
void GLWidget::resizeGL (int width, int height)
{
    glViewport(0, 0, width, height);
    cam_aspecto = static_cast<double>(width) / static_cast<double>(height);
    emit nuevoAR(cam_aspecto);
    actualizaAspecto();
}


//
// .: paintGL [protected] :.
//
// Función central de toda la aplicación.  Aquí se dibuja un frame coordinando
// todos las demás métodos importantes.
//
void GLWidget::paintGL (void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Miramos de marcar los visibles desde el robot seleccionad, si procede
    if (sel_marcavisibles)
        marcaVisibles();

    // Ahora empezamos a pintar
    glLoadIdentity();

    // Situamos las luces que se mueven con la cámara
    for (int i = 0; i < MAX_LUCES; i++)
        if (ilu_camara[i])
            glLightfv(GL_LIGHT0 + i, GL_POSITION, ilu_posicion[i]);

    // Ponemos la cámara en su sitio
    situaCamara();

    // Situamos las luces fijas en la escena
    for (int i = 0; i < MAX_LUCES; i++)
        if (!ilu_camara[i])
            glLightfv(GL_LIGHT0 + i, GL_POSITION, ilu_posicion[i]);

    // Pintamos el escenario
    escenario.pinta();

    // Pintamos los robots
    for (int i = 0; i < ani_nrobots; i++)
        if ((!cam_primera) || (i != sel_robot))
        {
            glPushMatrix();
            glTranslated(ani_probot[i].x, ani_arobots, ani_probot[i].z);
            glRotatef(ani_probot[i].angulo, 0.0, 1.0, 0.0);
            if (ani_probot[i].seleccionado)
            {
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,
                                     col_sel_amb[ani_probot[i].tipo_seleccion]);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,
                                     col_sel_dif[ani_probot[i].tipo_seleccion]);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
                                     col_sel_esp[ani_probot[i].tipo_seleccion]);
                // El brillo se mantiene fijo siempre para los seleccionados
                glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
            }
            robot.pinta(!ani_probot[i].seleccionado);
            glPopMatrix();
        }

    // Aquí acabamos de pintar, salvo que debamos mostrar el frustum
    if (!sel_pintafrustum || cam_primera || (sel_robot < 0))
        return;

    GLdouble mm[16];     // Matriz de Modelización
    GLdouble mp[16];     // Matriz de Proyección
    GLint    vp[4];      // ViewPort
    GLdouble cf[24];     // Coordenadas del Frustum

    double v1[3], v2[3]; // Vectores para calcular normales
    double n[3];         // Vector normal calculado

    // Ponemos la cámara en primera persona (modelview y projection)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    setPerspectiva(true);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    situaCamara(true);
    glGetDoublev(GL_PROJECTION_MATRIX, mp);
    glGetDoublev(GL_MODELVIEW_MATRIX,  mm);
    glGetIntegerv(GL_VIEWPORT,         vp);

    // Obtenemos las coordenadas del frustum (4 para znear y 4 para zfar)
    gluUnProject(vp[0], vp[1], 0.0, mm,mp,vp, &cf[0],  &cf[1],  &cf[2]);
    gluUnProject(vp[0], vp[1], 1.0, mm,mp,vp, &cf[3],  &cf[4],  &cf[5]);
    gluUnProject(vp[2], vp[1], 0.0, mm,mp,vp, &cf[6],  &cf[7],  &cf[8]);
    gluUnProject(vp[2], vp[1], 1.0, mm,mp,vp, &cf[9],  &cf[10], &cf[11]);
    gluUnProject(vp[2], vp[3], 0.0, mm,mp,vp, &cf[12], &cf[13], &cf[14]);
    gluUnProject(vp[2], vp[3], 1.0, mm,mp,vp, &cf[15], &cf[16], &cf[17]);
    gluUnProject(vp[0], vp[3], 0.0, mm,mp,vp, &cf[18], &cf[19], &cf[20]);
    gluUnProject(vp[0], vp[3], 1.0, mm,mp,vp, &cf[21], &cf[22], &cf[23]);

    // Ahora restauramos las matrices, lo podíamos haber hecho antes porque sólo
    // necesitábamos meterlas en nuestros arrays para el "gluUnProject()"
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Activamos el blending y definimos los materiales del frustum
    glEnable(GL_BLEND);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  col_frust_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  col_frust_dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, col_frust_esp);
    glBegin(GL_QUADS);

    // Ahora sí pintamos el frustum: 6 cuadrados
    v1[0] = cf[0]  - cf[6];
    v1[1] = cf[1]  - cf[7];
    v1[2] = cf[2]  - cf[8];
    v2[0] = cf[12] - cf[6];
    v2[1] = cf[13] - cf[7];
    v2[2] = cf[14] - cf[8];
    prodVectorial(v1, v2, n);

    glNormal3dv(n);
    glVertex3dv(&cf[0]);
    glVertex3dv(&cf[18]);
    glVertex3dv(&cf[12]);
    glVertex3dv(&cf[6]);

    v1[0] = cf[3]  - cf[9];
    v1[1] = cf[4]  - cf[10];
    v1[2] = cf[5]  - cf[11];
    v2[0] = cf[15] - cf[9];
    v2[1] = cf[16] - cf[10];
    v2[2] = cf[17] - cf[11];
    prodVectorial(v1, v2, n);

    glNormal3dv(n);
    glVertex3dv(&cf[3]);
    glVertex3dv(&cf[21]);
    glVertex3dv(&cf[15]);
    glVertex3dv(&cf[9]);

    v1[0] = cf[9]  - cf[6];
    v1[1] = cf[10] - cf[7];
    v1[2] = cf[11] - cf[8];
    v2[0] = cf[15] - cf[9];
    v2[1] = cf[16] - cf[10];
    v2[2] = cf[17] - cf[11];
    prodVectorial(v1, v2, n);

    glNormal3dv(n);
    glVertex3dv(&cf[12]);
    glVertex3dv(&cf[6]);
    glVertex3dv(&cf[9]);
    glVertex3dv(&cf[15]);

    v1[0] = cf[12] - cf[18];
    v1[1] = cf[13] - cf[19];
    v1[2] = cf[14] - cf[20];
    v2[0] = cf[15] - cf[12];
    v2[1] = cf[16] - cf[13];
    v2[2] = cf[17] - cf[14];
    prodVectorial(v1, v2, n);

    glNormal3dv(n);
    glVertex3dv(&cf[18]);
    glVertex3dv(&cf[12]);
    glVertex3dv(&cf[15]);
    glVertex3dv(&cf[21]);

    v1[0] = cf[3]  - cf[0];
    v1[1] = cf[4]  - cf[1];
    v1[2] = cf[5]  - cf[2];
    v2[0] = cf[9]  - cf[3];
    v2[1] = cf[10] - cf[4];
    v2[2] = cf[11] - cf[5];
    prodVectorial(v1, v2, n);

    glNormal3dv(n);
    glVertex3dv(&cf[0]);
    glVertex3dv(&cf[3]);
    glVertex3dv(&cf[9]);
    glVertex3dv(&cf[6]);

    v1[0] = cf[0]  - cf[3];
    v1[1] = cf[1]  - cf[4];
    v1[2] = cf[2]  - cf[5];
    v2[0] = cf[18] - cf[0];
    v2[1] = cf[19] - cf[1];
    v2[2] = cf[20] - cf[2];
    prodVectorial(v1, v2, n);

    glNormal3dv(n);
    glVertex3dv(&cf[18]);
    glVertex3dv(&cf[21]);
    glVertex3dv(&cf[3]);
    glVertex3dv(&cf[0]);
    glEnd();
    glDisable(GL_BLEND);
}


//
// .: prodVectorial [private] :.
//
// Calcula el producto vectorial de los vectores "u" y "v" colocando el
// resultado en el vector "n".  El producto vectorial de dos vectores nos
// proporcian el vector normal al plano que aquellos forman.
//
void GLWidget::prodVectorial (double u[3], double v[3], double n[3])
{
    n[0] = u[1] * v[2] - u[2] * v[1];
    n[1] = u[2] * v[0] - u[0] * v[2];
    n[2] = u[0] * v[1] - u[1] * v[0];
}

