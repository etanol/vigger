//
// luzcolor.cpp - Métodos auxiliares para gestionar luces y colores.
//
#include <GL/gl.h>
#include "glwidget.h"


//
// .: luzActivada [public] :.
//
// Devuelve verdadero si la luz "i" está activado, falso sino.
//
bool GLWidget::luzActivada (int i)
{
    GLboolean estat;
    glGetBooleanv(GL_LIGHT0 + i, &estat);
    return estat == GL_TRUE;
}


//
// .: activaLuz [public] :.
//
// Activa o desactiva la lux "i" según el parámetro "si".
//
void GLWidget::activaLuz (int i, bool si)
{
    if (si)
        glEnable(GL_LIGHT0 + i);
    else
        glDisable(GL_LIGHT0 + i);
    updateGL();
}


//
// .: getColLuz [public] :.
//
// Consulta el color de la componente "t" en la luz "i".  El resultado se
// transforma en un array de enteros del intervalo [0, 255] que se colocan en
// "col".
//
void GLWidget::getColLuz (int i, int t, int *col)
{
    GLenum componente;
    switch (t) {
        case  0: componente = GL_AMBIENT;  break;
        case  1: componente = GL_DIFFUSE;  break;
        case  2: componente = GL_SPECULAR; break;
        default: return;
    }

    float c[4];
    glGetLightfv(GL_LIGHT0 + i, componente, c);
    col[0] = static_cast<int>(c[0] * 255.0);
    col[1] = static_cast<int>(c[1] * 255.0);
    col[2] = static_cast<int>(c[2] * 255.0);
}


//
// .: setColLuz [public] :.
//
// Cambia el color de la componente "t" en la luz "i" según "r", "g" y "b" que
// son enteros en el intervalo [0, 255].  Cada byte de color se transforma en un
// float en [0, 1].
//
void GLWidget::setColLuz (int i, int t, int r, int g, int b)
{
    GLenum componente;
    switch (t) {
        case  0: componente = GL_AMBIENT;  break;
        case  1: componente = GL_DIFFUSE;  break;
        case  2: componente = GL_SPECULAR; break;
        default: return;
    }

    float c[4] = { colorf(r), colorf(g), colorf(b), 1.0 };
    glLightfv(GL_LIGHT0 + i, componente, c);
}


//
// .: setPosLuz [public] :.
//
// Coloca la luz "i" en la posición "x", "y" y "z".  Estas coordenadas serán
// respecto del observador (cámara) si "cam" es verdadero; en caso contrario las
// coordenadas son respecto del escenario.
//
void GLWidget::setPosLuz (int i, float x, float y, float z, bool cam)
{
    ilu_posicion[i][0] = x;
    ilu_posicion[i][1] = y;
    ilu_posicion[i][2] = z;
    ilu_posicion[i][3] = 1.0;
    ilu_camara[i]      = cam;
}


//
// .: setColFrust [public] :.
//
// Cambia el color del frustum visualizado.  Cada parámetro entero [0, 255] se
// transforma en float.  Después escalamos este color para cada componente
// (ambiente, difusa, especular) así basta con un método para cambiar este
// color.  Para simplificar la interfaz estas proporciones son fijas.
//
void GLWidget::setColFrust (int r, int g, int b)
{
    col_frust_amb[0] = colorf(r) * 0.01;
    col_frust_amb[1] = colorf(g) * 0.01;
    col_frust_amb[2] = colorf(b) * 0.01;

    col_frust_dif[0] = colorf(r) * 0.7;
    col_frust_dif[1] = colorf(g) * 0.7;
    col_frust_dif[2] = colorf(b) * 0.7;

    col_frust_esp[0] = 0.0;
    col_frust_esp[1] = 0.0;
    col_frust_esp[2] = 0.0;
}


//
// .: setColSelec [public] :.
//
// Cambia el color de los seleccionados.  Igual que antes los elementos de color
// se transforman en float y cada componente se pondera según un factor fijo.
// Ya que tenemos más de un tipo de selección, por usuario y visibles marcados,
// los discriminamos con el parámetro "t":
//
//     t = 0  ==>  Robot seleccionado por el usuario con el ratón.
//     t = 1  ==>  Robots visibles desde el robot seleccionado por el usuario.
//
void GLWidget::setColSelec (int t, int r, int g, int b)
{
    col_sel_amb[t][0] = colorf(r) * 0.01;
    col_sel_amb[t][1] = colorf(g) * 0.01;
    col_sel_amb[t][2] = colorf(b) * 0.01;
    col_sel_amb[t][3] = 1.0;

    col_sel_dif[t][0] = colorf(r) * 0.6;
    col_sel_dif[t][1] = colorf(g) * 0.6;
    col_sel_dif[t][2] = colorf(b) * 0.6;
    col_sel_dif[t][3] = 1.0;

    col_sel_esp[t][0] = colorf(r) * 0.2;
    col_sel_esp[t][1] = colorf(g) * 0.2;
    col_sel_esp[t][2] = colorf(b) * 0.2;
    col_sel_esp[t][3] = 1.0;
}


//
// .: setTrFrust [public slot] :.
//
// Modifica el nivel de transparencia del frustum.  El parámetro "trans" es un
// porcentaje de opacidad [0, 100] que se convierte en float.
//
void GLWidget::setTrFrust (int trans)
{
    float tr = static_cast<float>(trans) / 100.0;

    col_frust_amb[3] = tr;
    col_frust_dif[3] = tr;
    col_frust_esp[3] = tr;
    updateGL();
}


//
// .: pintaAlambre [public slot] :.
//
// Pinta la escena en modo alámbrico cuando el parámetro "si" es verdadero.
//
void GLWidget::pintaAlambre (bool si)
{
    if (si)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    updateGL();
}

