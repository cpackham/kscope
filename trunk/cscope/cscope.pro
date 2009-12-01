include(../config)
TEMPLATE = lib
TARGET = kscope_cscope
DEPENDPATH += ". ../core"
CONFIG += dll

# Input
HEADERS += engineconfigwidget.h \
    ctags.h \
    configwidget.h \
    managedproject.h \
    crossref.h \
    cscope.h \
    files.h \
    grep.h
FORMS += configwidget.ui \
    engineconfigwidget.ui
SOURCES += engineconfigwidget.cpp \
    ctags.cpp \
    configwidget.cpp \
    managedproject.cpp \
    crossref.cpp \
    cscope.cpp \
    files.cpp \
    grep.cpp
INCLUDEPATH += .. \
    .
LIBS += -L../core -lkscope_core
target.path = $${INSTALL_PATH}/lib
INSTALLS += target
