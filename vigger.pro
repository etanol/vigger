TEMPLATE = app
LANGUAGE = C++

CONFIG  += qt warn_on release

LIBS    += -L. -lGL -lGLU -l3ds

HEADERS += glwidget.h \
           vdisplay.h \
           modelo.h \
           vpushbutton.h

SOURCES += vigger_main.cpp \
           glwidget.cpp \
           glfuncs.cpp \
           eventos.cpp \
           camara.cpp \
           animacion.cpp \
           seleccion.cpp \
           modelo.cpp \
           luzcolor.cpp

FORMS    = ayuda.ui \
           ventana.ui

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

