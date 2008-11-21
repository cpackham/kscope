# #####################################################################
# Automatically generated by qmake (2.01a) Thu Apr 24 18:03:58 2008
# #####################################################################
TEMPLATE = app
TARGET = kscopeapp
DEPENDPATH += """"". ../core ../cscope"""""
CONFIG += debug_and_release \
    warn_all
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Input
SOURCES += findtextdialog.cpp \
    projectmanager.cpp \
    editorconfigdialog.cpp \
    projectdialog.cpp \
    projectfilesdialog.cpp \
    stackwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    actions.cpp \
    application.cpp \
    querydialog.cpp \
    editorcontainer.cpp \
    editor.cpp \
    queryresultdock.cpp \
    queryresultdialog.cpp \
    addfilesdialog.cpp
HEADERS += strings.h \
    locationhistory.h \
    findtextdialog.h \
    projectmanager.h \
    editorconfigdialog.h \
    projectfilesdialog.h \
    stackwidget.h \
    mainwindow.h \
    actions.h \
    application.h \
    querydialog.h \
    editorcontainer.h \
    editor.h \
    fileiothread.h \
    queryresultdock.h \
    queryresultdialog.h \
    addfilesdialog.h \
    projectdialog.h \
    buildprogress.h
FORMS += querydialog.ui \
    queryresultdialog.ui \
    stackpage.ui \
    projectfilesdialog.ui \
    addfilesdialog.ui \
    projectdialog.ui \
    editorconfigdialog.ui \
    findtextdialog.ui
INCLUDEPATH += ../cscope \
    ../core \
    /usr/include/Qsci \
    .
LIBS += -L../core \
    -lkscope_core \
    -L../cscope \
    -lkscope_cscope \
    -lqscintilla2
TARGETDEPS += 
RESOURCES = ide.qrc
