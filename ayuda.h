//
// ayuda.h - Conexión del formulario ayuda.ui con el resto del código.
//
// Ver fichero ventana.h para una explicación más detallada.
//
#ifndef _AYUDA_H_
#define _AYUDA_H_

#include <QDialog>
#include "ui_ayuda.h"

class Ayuda : public QDialog, private Ui::Ayuda
{
    Q_OBJECT

public:

    Ayuda (QWidget *parent);

};

#endif // _AYUDA_H_
