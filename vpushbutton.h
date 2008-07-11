//
// vpushbutton.h - Extensión de la clase QPushButton para mostrar el color
//                 seleccionado.
//
#ifndef _VPUSHBUTTON_H_
#define _VPUSHBUTTON_H_

#include <QPushButton>


class VPushButton : public QPushButton {

    Q_OBJECT

public:

    VPushButton (QWidget *parent)
            : QPushButton(parent)
    {
        r = 0; g = 0; b = 0;
        updateColor();
    }

    inline int getR () { return r; }
    inline int getG () { return g; }
    inline int getB () { return b; }

public slots:

    inline void setR (int n) { r = n; updateColor(); }
    inline void setG (int n) { g = n; updateColor(); }
    inline void setB (int n) { b = n; updateColor(); }

    inline void setRGB (int pr, int pg, int pb)
                { r = pr; g = pg; b = pb; updateColor(); }

private:

    int r, g, b;

    void updateColor ()
    {
        QPalette p = palette();
        p.setColor(backgroundRole(), QColor(r, g, b));
        setPalette(p);
    }

};

#endif
