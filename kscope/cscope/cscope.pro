include(../config)
TEMPLATE = lib
TARGET = kscope_cscope
DEPENDPATH += ". ../core"
CONFIG += dll

# Input
HEADERS += ctags.h \
    configwidget.h \
    managedproject.h \
    crossref.h \
    cscope.h \
    files.h
FORMS += configwidget.ui
SOURCES += ctags.cpp \
    configwidget.cpp \
    managedproject.cpp \
    crossref.cpp \
    cscope.cpp \
    files.cpp
INCLUDEPATH += ../core \
    .
target.path = $${INSTALL_PATH}/lib
INSTALLS += target
