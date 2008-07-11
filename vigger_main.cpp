//
// vigger_main.cpp - Función principal.
//
// Implementación trivial.  Traspasa toda la responsabilidad a la clase Ventana.
//
#include <QApplication>
#include "ventana.h"

int main (int argc, char **argv)
{
    QApplication a(argc, argv);
    Ventana w;
    w.show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}

