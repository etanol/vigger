TEMPLATE = app
LANGUAGE = C++

CONFIG  += qt warn_on debug
QT      += opengl

LIBS    += -L. -lGL -lGLU -l3ds

HEADERS += ayuda.h \
           glwidget.h \
           vdisplay.h \
           modelo.h \
           vpushbutton.h \
           ventana.h

SOURCES += vigger_main.cpp \
           ayuda.cpp \
           glwidget.cpp \
           glfuncs.cpp \
           eventos.cpp \
           camara.cpp \
           animacion.cpp \
           seleccion.cpp \
           modelo.cpp \
           luzcolor.cpp \
           ventana.cpp

FORMS    = ayuda.ui \
           ventana.ui

unix {
  UI_DIR      = .ui
  MOC_DIR     = .moc
  OBJECTS_DIR = .obj
}

