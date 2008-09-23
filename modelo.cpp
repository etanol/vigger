//
// modelo.cpp - Implementación de la carga de modelos 3DS.
//
// El trabajo sucio lo hace la lib3ds.
//
#include <limits>
#include <cmath>
#include <GL/gl.h>
#include <lib3ds/node.h>
#include <lib3ds/mesh.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/material.h>
#include "modelo.h"


// Colores por defecto
const float Modelo::L_BRILLO_DEF= 0.0;
const float Modelo::L_AMBIENTE_DEF[4]  = { 0.1, 0.0, 0.2, 1.0 };
const float Modelo::L_DIFFUSO_DEF[4]  = { 0.2, 0.0, 0.4, 1.0 };
const float Modelo::L_ESPECULAR_DEF[4] = { 0.0, 0.0, 0.0, 1.0 };


// Constructor
Modelo::Modelo () : fichero(0),
                    radio_esfera(0.0),   radio_esfera0(0.0),
                    radio_xz(0.0),       radio_xz0(0.0),
                    x_max(0.0),          x_max0(0.0),
                    y_max(0.0),          y_max0(0.0),
                    z_max(0.0),          z_max0(0.0),
                    hacer_escala(false), hacer_rotacion(false),
                    factor_escala(1.0),  angulo_rotacion(0)
{ }


//
// carga - Carga un nuevo modelo desde un fichero.  Si algo va mal devuelve
//         falso, sino devuelve verdadero.
//
bool Modelo::carga (const char *nombre)
{
    Lib3dsFile *f;

    f = lib3ds_file_load(nombre);
    if (f == 0)
        return false;

    if (fichero != 0)
        lib3ds_file_free(fichero);
    fichero = f;

    lib3ds_file_eval(fichero, 0);
    computaEsferaEnvoltorio();
    return true;
}


//
// computaEsferaEnvoltorio - Hace un recorrido por todos los puntos definidos
//                           para encontrar los valores máximos y mínimos de x,
//                           y, z para así calcular el radio de la esfera
//                           envolvente.
//
void Modelo::computaEsferaEnvoltorio ()
{
    Lib3dsMesh *mesh;
    double minx, miny, minz;
    double maxx, maxy, maxz;
    double centrox, centroy, centroz;

    minx = miny = minz =  __DBL_MAX__;
    maxx = maxy = maxz =  __DBL_MIN__;

    // Buscamos los puntos máximos y mínimos
    mesh = fichero->meshes;
    while (mesh != 0)
    {
        for(unsigned int i = 0; i < mesh->points; i++)
        {
            double x = mesh->pointL[i].pos[0];
            double y = mesh->pointL[i].pos[1];
            double z = mesh->pointL[i].pos[2];

            minx = (minx > x ? x : minx);
            miny = (miny > y ? y : miny);
            minz = (minz > z ? z : minz);

            maxx = (maxx < x ? x : maxx);
            maxy = (maxy < y ? y : maxy);
            maxz = (maxz < z ? z : maxz);
        }
        mesh = mesh->next;
    }

    // Buscamos el centro
    centrox = (minx + maxx) / 2.0;
    centroy = (miny + maxy) / 2.0;
    centroz = (minz + maxz) / 2.0;

    // Y el radio de la esfera
    double dx, dy, dz;
    dx = maxx - centrox;
    dy = maxy - centroy;
    dz = maxz - centroz;
    radio_esfera  = sqrt(dx*dx + dy*dy + dz*dz);
    radio_esfera0 = radio_esfera;

    // Ahora normalizamos el modelo y lo ponemos en el origen de coordenadas
    minx -= centrox; miny -= centroy; minz -= centroz;
    maxx -= centrox; maxy -= centroy; maxz -= centroz;

    mesh = fichero->meshes;
    while (mesh != 0)
    {
        for (unsigned int i = 0; i < mesh->points; i++)
        {
            mesh->pointL[i].pos[0] -= centrox;
            mesh->pointL[i].pos[1] -= centroy;
            mesh->pointL[i].pos[2] -= centroz;
        }
        mesh = mesh->next;
    }

    // Ya está todo en su sitio.  Sólo hace falta memorizar los límites
    // positivos porque ya tenemos el modelo centrado en el origen de
    // coordenadas.  Entonces:
    //     x_min = -x_max
    //     y_min = -y_max
    //     z_min = -z_max
    x_max0 = x_max = maxx;
    y_max0 = y_max = maxy;
    z_max0 = z_max = maxz;

    // Finalmente calculamos el radio del plano XZ
    radio_xz  = sqrt(x_max*x_max + z_max*z_max);
    radio_xz0 = radio_xz;
}


//
// escala - Modifica el tamaño del modelo según "factor".
//
void Modelo::escala (double factor)
{
    if (fichero == 0)
        return;

    hacer_escala  = true;
    factor_escala = factor;

    // Los radios y los máximos también cambian
    radio_esfera *= factor;
    radio_xz     *= factor;
    x_max        *= factor;
    y_max        *= factor;
    z_max        *= factor;
}


//
// rota - Gira el modelo para ajustar la orientación
//
void Modelo::rota (int angulo)
{
    if (angulo == 0)
        hacer_rotacion = false;
    else
    {
        hacer_rotacion  = true;
        angulo_rotacion = angulo;
    }
}


//
// redimensiona - Cambia el radio del modelo a "nuevo_radio".
//
void Modelo::redimensiona (double nuevo_radio)
{
    if (fichero == 0)
        return;

    // Si habíamos escalado antes, volvemos a los valores originales
    if (hacer_escala)
    {
        radio_esfera = radio_esfera0;
        radio_xz     = radio_xz0;
        x_max        = x_max0;
        y_max        = y_max0;
        z_max        = z_max0;
    }

    escala(nuevo_radio / radio_esfera);
}


//
// pinta - Pinta el modelo entero, a saco.  Si "con_color" es verdadero se
//         utilizarán los colores del modelo, si los hay.  Si "con_normales" es
//         verdadero se utilizarán las normales del modelo.
//
void Modelo::pinta (bool con_color, bool con_normales)
{
    if (fichero == 0)
        return;

    if (hacer_escala)
        glScaled(factor_escala, factor_escala, factor_escala);
    if (hacer_rotacion)
        glRotatef(static_cast<GLfloat>(angulo_rotacion), 0.0, 1.0, 0.0);

    Lib3dsMesh *mesh = fichero->meshes;
    while (mesh != 0)
    {
        glBegin(GL_TRIANGLES);
        for (unsigned int i = 0; i < mesh->faces; i++)
        {
            Lib3dsFace *f = &mesh->faceL[i];

            if (con_color)
            {
                Lib3dsMaterial *m = lib3ds_file_material_by_name(fichero, f->material);
                if (m)
                {
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   m->ambient);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   m->diffuse);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m->specular);
                    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m->shininess);
                }
                else
                {
                    // No hay color para esta cara; mala suerte, ponemos los
                    // colores por defecto
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   L_AMBIENTE_DEF);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   L_DIFFUSO_DEF);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  L_ESPECULAR_DEF);
                    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, L_BRILLO_DEF);
                }
            }

            if (con_normales)
                glNormal3fv(f->normal);

            // Y todo esto para un triángulo, uff...
            glVertex3fv(mesh->pointL[f->points[0]].pos);
            glVertex3fv(mesh->pointL[f->points[1]].pos);
            glVertex3fv(mesh->pointL[f->points[2]].pos);
        }
        glEnd();
        mesh = mesh->next;
    }
}

