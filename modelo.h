//
// modelo.h - Clase para manipular modelos 3DS por separado de la escena.
//
#ifndef _MODELO_H_
#define _MODELO_H_

#include <lib3ds/file.h>

class Modelo {

public:

    Modelo ();
    bool carga        (const char *nombre);
    void escala       (double factor);
    void rota         (int angulo);
    void pinta        (bool con_color = true, bool con_normales = true);
    void redimensiona (double nuevo_radio);

    // Los campos sólo se pueden consultar, por tanto sólo programamos gets
    inline double radio   () const { return radio_esfera; }
    inline double radioXZ () const { return radio_xz; }
    inline double maxX    () const { return x_max; }
    inline double maxY    () const { return y_max; }
    inline double maxZ    () const { return z_max; }

private:

    void computaEsferaEnvoltorio ();

    // Los campos acabados en 0 son los originales
    Lib3dsFile *fichero;
    double      radio_esfera, radio_esfera0;
    double      radio_xz, radio_xz0;
    double      x_max, x_max0;
    double      y_max, y_max0;
    double      z_max, z_max0;
    bool        hacer_escala;
    bool        hacer_rotacion;
    double      factor_escala;
    int         angulo_rotacion;

    // Colores de luz por defecto
    static const float L_BRILLO_DEF;
    static const float L_AMBIENTE_DEF[4];
    static const float L_DIFFUSO_DEF[4];
    static const float L_ESPECULAR_DEF[4];
};

#endif /* _MODELO_H_ */
