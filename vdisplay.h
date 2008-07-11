//
// vdisplay.h - Extensión de la clase QLineEdit para poder mostrar números desde
//              los slots, que siempre es más cómodo.
//
#ifndef _VDISPLAY_H_
#define _VDISPLAY_H_

#include <QLineEdit>

class VDisplay : public QLineEdit
{
    Q_OBJECT

public:

    VDisplay (QWidget *parent)
            : QLineEdit(parent)
    {
        // Adornamos un poco
        setFrame(FALSE);
        setMaximumWidth(50);
        setEnabled(FALSE);
    }

public slots:

    inline void display (int    num) { setText(QString::number(num)); }
    inline void display (float  num) { display(static_cast<double>(num)); }
    inline void display (unsigned int num) { setText(QString::number(num)); }
    inline void display (double num)
        { setText(QString::number(num, 'f', 2)); }

    inline void displayFloat (int num)
        { display(static_cast<double>(num) / 10.0); }
    inline void displayFloat (unsigned int num)
        { display(static_cast<double>(num) / 10.0); }

};

#endif // _VDISPLAY_H_
