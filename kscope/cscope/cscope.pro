# #####################################################################
# Automatically generated by qmake (2.01a) Thu Apr 24 17:48:31 2008
# #####################################################################
TEMPLATE = lib
TARGET = kscope_cscope
DEPENDPATH += ". ../core"
CONFIG += debug_and_release \
    warn_all \
    dll
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Input
HEADERS += configwidget.h \
    managedproject.h \
    crossref.h \
    cscope.h \
    files.h
FORMS += configwidget.ui
SOURCES += configwidget.cpp \
    managedproject.cpp \
    crossref.cpp \
    cscope.cpp \
    files.cpp
INCLUDEPATH += ../core \
    .
