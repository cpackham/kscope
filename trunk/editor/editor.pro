include(../config)
TEMPLATE = lib
TARGET = kscope_editor
DEPENDPATH += ". ../core"
CONFIG += dll

# Input
HEADERS += editor.h \
    configdialog.h \
    fileiothread.h \
    findtextdialog.h \
    config.h
FORMS += configdialog.ui \
    findtextdialog.ui
SOURCES += config.cpp \
    editor.cpp \
    configdialog.cpp \
    findtextdialog.cpp
INCLUDEPATH += .. \
    $${QSCI_ROOT_PATH}/include/Qsci \
    .
LIBS += -L../core \
    -lkscope_core \
    -L$${QSCI_ROOT_PATH}/lib \
    -lqscintilla2
target.path = $${INSTALL_PATH}/lib
INSTALLS += target
