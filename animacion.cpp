//
// animacion.cpp - Funciones relativas a la parte de la animación.
//
#include <climits>
#include <cmath>
#include "glwidget.h"


//
// .: setNumRobots [public slot] :.
//
// Cambia el número de robots de la escena. Cuando se añaden robots se hace poco
// a poco para asegurar que se sitúa cada robot en una posición segura. Si
// eliminamos robots hay que ir con más cuidado porque el robot seleccionado
// puede ser uno de ellos. Para no complicar demasiado este último caso
// directamente deseleccionaremos el robot.
//
void GLWidget::setNumRobots (int num)
{
    num = (num > MAX_ROBOTS ? MAX_ROBOTS : num);
    if (num <= ani_nrobots) {
        ani_nrobots = num;
        if (sel_robot >= ani_nrobots) {
            ani_probot[sel_robot].seleccionado = false;
            sel_robot   = -1;
            cam_primera = false;
            // Desmarcamos los visibles, si es necesario
            if (sel_marcavisibles)
                for (int i = 0; i < ani_nrobots; i++)
                    ani_probot[i].seleccionado = false;
            emit nuevaSel(sel_robot);
        }
    } else {
        // El incremento se debe hacer poco a poco para asegurarnos de colocar
        // cada robot correctamente
        while (ani_nrobots < num) {
            ani_nrobots++;
            nuevoRobot(ani_nrobots - 1);
        }
    }
    updateGL();
}


//
// .: setFPS [public slot] :.
//
// Aparte de la velocidad lineal también se puede hacer que la animación vaya
// más suave aumentando los frames por segundo con una reducción del intervalo
// de tiempo entre cada "timerEvent()"; que viene dado en milisegundos. Si la
// animación está activada, tenemos que destruir el "timer" activo y crear uno
// nuevo con el intervalo actualizado.
//
void GLWidget::setFPS (int fps)
{
    ani_tinterval = 1000 / fps;
    if (ani_timer) {
        killTimer(ani_timer);
        ani_timer = startTimer(ani_tinterval);
    }
}


//
// .: animaRobots [public slot] :.
//
// Inicia o detiene la animación de los robots. Cuando la detenemos, el
// identificador se pone a cero para que cuando modifiquemos los frames por
// segundo podamos saber si la escena está animada o no en ese momento.
//
void GLWidget::animaRobots (bool si)
{
    if (si) {
        ani_timer = startTimer(ani_tinterval);
    } else {
        killTimer(ani_timer);
        ani_timer = 0;
    }
}


//
// .: timerEvent [protected virtual] :.
//
// Animación de los robots. En condiciones normales el robot avanza, hasta que
// se detecta una colisión (ya sea contra el límite de la escena o contra otro
// robot) entonces se asigna una nueva dirección de movimiento (aleatoria) y se
// empieza a girar hacia esta nueva dirección.
//
void GLWidget::timerEvent (QTimerEvent *ev)
{
    int    sig;
    double tmpx, tmpz;

    for (int i = 0; i < ani_nrobots; i++) 
        if (ani_probot[i].gira) {
            // El robot gira
            sig = signo(ani_probot[i].angulo_destino - ani_probot[i].angulo);
            ani_probot[i].angulo += sig * ani_vangular;
            if (sig!=signo(ani_probot[i].angulo_destino-ani_probot[i].angulo)) {
                // Nos hemos pasado de giro, toca avanzar
                ani_probot[i].angulo = ani_probot[i].angulo_destino;
                double rads = radianes(ani_probot[i].angulo);
                ani_probot[i].cos_angulo = cos(rads) * ani_avance;
                ani_probot[i].sin_angulo = sin(rads) * ani_avance;
                ani_probot[i].gira       = false;
            }
        } else {
            // El robot avanza
            tmpx = ani_probot[i].x + ani_probot[i].sin_angulo;
            tmpz = ani_probot[i].z + ani_probot[i].cos_angulo;
            if (posicionValida(i, tmpx, tmpz)) {
                // Posición correcta, avanzamos
                ani_probot[i].x = tmpx;
                ani_probot[i].z = tmpz;
            } else {
                // Colisión, cambio de dirección
                ani_probot[i].angulo_destino = random() % 360;
                ani_probot[i].gira           = true;
            }
        }
    updateGL(); // Y repintamos
}


//
// .: ajustaVLineal [private] :.
//
// La longitud del avance (es decir, la velocidad lineal real) se calcula en
// función del factor "ani_vlineal", donde una unidad de "ani_vlineal"
// representa un 1% del radio horizontal del escenario.
//
// Dado que memorizamos los cálculos trigonométricos para reducir tiempo de
// computación, si queremos que el cambio de velocidad lineal se refelje
// instantáneamente hay que recalcular los valores memorizados para todos los
// robots necesarios.
//
void GLWidget::ajustaVLineal ()
{
    double rads;

    ani_avance = decimal(ani_vlineal) * escenario.radioXZ() * 0.1;
    for (int i = 0; i < ani_nrobots; i++)
        if (!ani_probot[i].gira) {
            // Sólo si el robot avanza
            rads = radianes(ani_probot[i].angulo);
            ani_probot[i].cos_angulo = cos(rads) * ani_avance;
            ani_probot[i].sin_angulo = sin(rads) * ani_avance;
        }
}


//
// .: nuevoRobot [private] :.
// 
// Inicializa el robot representado por "indice" dándole posición y dirección
// inicial aleatorias. Hay que vigilar que estos valores sean correctos para
// evitar las colisiones.
//
void GLWidget::nuevoRobot (int indice)
{
    double limitex = escenario.maxX() * (2.0 / static_cast<double>(RAND_MAX));
    double limitez = escenario.maxZ() * (2.0 / static_cast<double>(RAND_MAX));
    double tmpx, tmpz;

    do {
        tmpx = static_cast<double>(random()) * limitex;
        tmpz = static_cast<double>(random()) * limitez;
        tmpx -= escenario.maxX();
        tmpz -= escenario.maxZ();
    } while (!posicionValida(indice, tmpx, tmpz));
    // La posición ya es correcta (sin colisión), completamos los campos
    // importantes de la estructura
    ani_probot[indice].angulo       = random() % 360;
    ani_probot[indice].gira         = false;
    ani_probot[indice].seleccionado = false;
    ani_probot[indice].x            = tmpx;
    ani_probot[indice].z            = tmpz;
    double rads = radianes(ani_probot[indice].angulo);
    ani_probot[indice].cos_angulo = cos(rads) * ani_avance;
    ani_probot[indice].sin_angulo = sin(rads) * ani_avance;
}


//
// .: posicionValida [private] :.
//
// Dado un robot representado por "indice" del vector de robots "ani_probot" y
// unas coorenadas "x", "z" devuelve verdadero cuando estas coordenadas son
// seguras para pintar el robot, es decir, no colisionaría con otros robots o
// con los límites del escenario.
//
bool GLWidget::posicionValida (int indice, double x, double z)
{
    double dist, dx, dz;
    double radio2 = robot.radioXZ() * 2.0;

    // Si cae fuera de la caja envolvente (escenario) no vale: devuelve falso
    dx = fabs(x);
    dz = fabs(z);
    if ((dx > (escenario.maxX() - robot.radioXZ()))
            || (dz > (escenario.maxZ() - robot.radioXZ())))
        return false;
    // Ahora comprobamos que no pise a ningún otro robot, salvo él mismo claro
    for (int i = 0; i < ani_nrobots; i++) 
        if (i != indice) {
            dx   = ani_probot[i].x - x;
            dz   = ani_probot[i].z - z;
            dist = sqrt((dx*dx) + (dz*dz));
            if (dist < radio2)
                return false;
        }
    // Prueba superada
    return true;
}

