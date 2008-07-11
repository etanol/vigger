#include <qapplication.h>
#include "ventana.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    Ventana w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
