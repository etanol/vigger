//
// vpushbutton.h - Extensión de la clase QPushButton para mostrar el color
//                 seleccionado.
//
#ifndef _VPUSHBUTTON_H_
#define _VPUSHBUTTON_H_

#include <qpushbutton.h>
#include <qcolor.h>

#define BYTE_MASK 0x000000FF

class VPushButton : public QPushButton {

    Q_OBJECT

public:

    VPushButton (QWidget *parent, const char *name = 0)
            : QPushButton(parent, name)
    {
        r = 0; g = 0; b = 0;
        setPaletteBackgroundColor(QColor(r, g, b));
    }

    inline int getR () { return r; }
    inline int getG () { return g; }
    inline int getB () { return b; }

public slots:

    inline void setR (int n)
        { r = n & BYTE_MASK; setPaletteBackgroundColor(QColor( r, g, b )); }
    inline void setG (int n)
        { g = n & BYTE_MASK; setPaletteBackgroundColor(QColor( r, g, b )); }
    inline void setB (int n)
        { b = n & BYTE_MASK; setPaletteBackgroundColor(QColor( r, g, b )); }

    inline void setRGB (int pr, int pg, int pb)
        { r = pr; g = pg; b = pb; setPaletteBackgroundColor(QColor(r, g, b)); }

private:

    int r, g, b;

};

#endif
