# #####################################################################
# Automatically generated by qmake (2.01a) Thu Apr 24 17:48:10 2008
# #####################################################################
TEMPLATE = lib
TARGET = kscope_core
CONFIG += debug_and_release \
    warn_all \
    dll
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Input
HEADERS += locationmodel.h \
    projectconfig.h \
    filescanner.h \
    filefilter.h \
    queryview.h \
    locationlistmodel.h \
    parser.h \
    exception.h \
    project.h \
    codebase.h \
    codebasemodel.h \
    globals.h \
    process.h \
    statemachine.h \
    treeitem.h \
    progressbar.h \
    engine.h
FORMS += progressbar.ui
SOURCES += locationmodel.cpp \
    filescanner.cpp \
    queryview.cpp \
    locationlistmodel.cpp \
    codebasemodel.cpp \
    process.cpp \
    progressbar.cpp
