//
// glwidget.h - Extensión de QGLWidget para nuestra aplicación.
//
// Dado que esta clase es muy extensa y que los métodos se pueden agrupar
// semánticamente, las implementaciones están distribuídas en varios ficheros.
// Al lado de cada método declarado hay un comentario indicando en qué fichero
// se implementa. Si no hay ningún comentario la implementación se encuentra en
// "glwidget.cpp".
//
#ifndef _GLWIDGET_H_
#define _GLWIDGET_H_

#include <QGLWidget>
#include <cmath>
#include "modelo.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:

    GLWidget  (QWidget *parent);

    // Métodos para cargar modelos
    bool cargaEscenario (QString &fichero);
    bool cargaRobots    (QString &fichero);

    // Métodos para gestionar luces y colores (luzcolor.cpp)
    bool luzActivada (int i);
    void activaLuz   (int i, bool si);
    void setColLuz   (int i, int t, int r, int g, int b);
    void getColLuz   (int i, int t, int *col);
    void setPosLuz   (int i, float x, float y, float z, bool cam);
    void setColFrust (int r, int g, int b);
    void setColSelec (int t, int r, int g, int b);
    void setColVisi  (int r, int g, int b);

    inline float *getPosLuz (int i) { return ilu_posicion[i]; }
    inline bool   getRefLuz (int i) { return ilu_camara[i];   }
    inline void   setColorFondo (int r, int g, int b)
        { glClearColor(colorf(r), colorf(g), colorf(b), 1.0); }

signals:

    void nuevoAnguloX (int angulo);
    void nuevoAnguloY (int angulo);
    void nuevoAnguloZ (int angulo);
    void nuevoAnguloA (int angulo);
    void nuevaDist    (int dist);
    void nuevaSel     (int indice);
    void nuevoZNear   (double znear);
    void nuevoZFar    (double zfar);
    void nuevoPanX    (double panx);
    void nuevoPanY    (double pany);
    void nuevoAR      (double ar);
    void nuevoRadio   (double radio);
    void nuevoRadioXZ (double radio);
    void alturas      (int emaxy, int rmaxy);

public slots:

    // Los sencillos primero
    inline void setAnguloX    (int a)  { cam_angulox = a; updateGL();          }
    inline void setAnguloY    (int a)  { cam_anguloy = a; updateGL();          }
    inline void setAnguloZ    (int a)  { cam_anguloz = a; updateGL();          }
    inline void setApertura   (int a)  { cam_anguloa0 = a; ajustaApertura(); updateGL(); }
    inline void setDistancia  (int d)  { setDistancia(decimal(d));             }
    inline void setVAngular   (int v)  { ani_vangular = v;                     }
    inline void setVLineal    (int v)  { ani_vlineal = v; ajustaVLineal();     }
    inline void setAltura     (int a)  { ani_arobots = decimal(a); updateGL(); }
    inline void setRotacionRob(int r)  { robot.rota(r * 90); updateGL();       }
    inline void setPintaFrus  (bool s) { sel_pintafrustum = s; updateGL();     }

    // Ahora los no tan sencillos :-P
    void reseteaCam    ();            // -+- camara.cpp
    void setCamPrimera (bool si);     //  |
    void setZFarCamPri (int z);       //  |
    void setDistancia  (double dist); // _/
    void setNumRobots  (int num);     // -+- animacion.cpp
    void setFPS        (int fps);     //  |
    void animaRobots   (bool si);     // _/
    void setMarcaVis   (bool si);     // --- seleccion.cpp
    void setTrFrust    (int trans);   // -+- luzcolor.cpp
    void pintaAlambre  (bool si);     // _/

protected:

    // Métodos a redefinir
    void initializeGL ();                      // -+- glfuncs.cpp
    void paintGL      ();                      //  |
    void resizeGL     (int width, int height); // _/

    // Eventos gestionados
    void timerEvent      (QTimerEvent *ev); // --- animacion.cpp
    void keyPressEvent   (QKeyEvent *ev);   // -+- eventos.cpp
    void mousePressEvent (QMouseEvent *ev); //  |
    void mouseMoveEvent  (QMouseEvent *ev); //  |
    void wheelEvent      (QWheelEvent *ev); // _/

private:

    // Constantes
    static const int MAX_ROBOTS = 20;
    static const int MAX_LUCES  = 3;

    // Métodos auxiliares triviales
    inline int    entero  (double n) { return static_cast<int>(n * 10.0);      }
    inline int    signo   (int    n) { return (n >> ((sizeof(int)*8)-1)) | 1;  }
    inline double decimal (int    n) { return static_cast<double>(n) * 0.1;    }
    inline double grados  (double r) { return r * (180.0 / M_PI);              }
    inline double radianes(double g) { return g * (M_PI / 180.0);              }
    inline double radianes(int    g) { return radianes(static_cast<double>(g));}
    inline float  colorf  (int    c) { return static_cast<float>(c) / 255.0;   }

    // Métodos auxiliares
    void ajustaRecorteZ     ();                             // -+- camara.cpp
    void ajustaApertura     ();                             //  |
    void actualizaAspecto   ();                             //  |
    void rectificaRotaciones();                             //  |
    void situaCamara        (bool forzar_prim = false);     //  |
    void setPerspectiva     (bool forzar_prim = false);     // _/
    void ajustaVLineal      ();                             // -+- animacion.cpp
    void nuevoRobot         (int index);                    //  |
    bool posicionValida     (int index, double x, double z);// _/
    void seleccionaRobot    (int x, int y);                 // -+- seleccion.cpp
    void marcaVisibles      ();                             // _/
    void prodVectorial (double u[3], double v[3], double n[3]); // glfuncs.cpp


    //////////////////////////
    // Modelos de la escena //
    //////////////////////////
    Modelo escenario;
    Modelo robot;

    ///////////////////////////////////
    // Campos referentes a la camara //
    ///////////////////////////////////
    int    cam_angulox;   // Ángulos de rotación en 3a persona
    int    cam_anguloy;
    int    cam_anguloz;
    int    cam_anguloa0;  // Ángulo original de apertura vertical
    double cam_panx;
    double cam_pany;
    double cam_anguloa;   // Ángulo real de apertura vertical
    double cam_aspecto;   // Relación de aspecto del viewport
    double cam_znear;     // Planos de recorte
    double cam_zfar;
    double cam_zfarpri;
    double cam_avance;    // Factor de movilidad
    double cam_distancia; // Distancia del observador a la escena
    bool   cam_primera;   // Flag para seleccionar 1a o 3a persona

    //////////////////////////////////////
    // Campos referentes a la animación //
    //////////////////////////////////////

    // Esta estructura indica la posición actual y la dirección de movimiento de
    // un robot en particular. También se memorizan algunos valores para reducir
    // la carga computacional.
    struct PosRobot
    {
        double cos_angulo;
        double sin_angulo;
        double x;
        double z;
        int    angulo;
        int    angulo_destino;
        int    tipo_seleccion;
        bool   seleccionado;
        bool   gira;
    };

    PosRobot ani_probot[MAX_ROBOTS];
    double   ani_arobots;   // Elevación de los robots
    double   ani_avance;    // Distancia de avance por paso según "ani_vlineal"
    int      ani_vlineal;   // Velocidad lineal
    int      ani_vangular;  // Velocidad angular
    int      ani_nrobots;   // Total de robots en la escena
    int      ani_timer;     // Identificador del timer activo (0 si ninguno)
    int      ani_tinterval; // Intervalo de tiempo entre frames (1000 / FPS)

    //////////////////////////////////////
    // Campos referentes a la selección //
    //////////////////////////////////////
    int  sel_robot;         // Índice del robot seleccionado (-1 si ninguno)
    bool sel_marcavisibles; // Visibles desde el robot seleccionado
    bool sel_pintafrustum;  // Frustum de visión del robot seleccionado

    /////////////////////
    // Campos de color //
    /////////////////////
    float col_frust_amb[4];  // Frustum ambiente
    float col_frust_dif[4];  // Frustum difuso
    float col_frust_esp[4];  // Frustum especular
    float col_sel_amb[2][4]; // Seleccionado ambiente
    float col_sel_dif[2][4]; // Seleccionado difuso
    float col_sel_esp[2][4]; // Seleccionado especular

    ///////////////////////////
    // Campos de iluminación //
    ///////////////////////////
    float ilu_posicion[MAX_LUCES][4]; // Posición de la luz
    bool  ilu_camara[MAX_LUCES];      // Referencia de la posición

    ///////////////////////
    // Campos auxiliares //
    ///////////////////////
    int    aux_rx; // Estos campos nos ayudarán con los eventos de ratón
    int    aux_ry;
    int    aux_angulox;
    int    aux_anguloy;
    double aux_panx;
    double aux_pany;

    // Color del frustum de visión de un robot
    static const float cfru_ambient[4];
    static const float cfru_diffuse[4];
    static const float cfru_specular[4];

};

#endif // _GLWIDGET_H_
