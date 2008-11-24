include(../config)
TEMPLATE = app
TARGET = kscopeapp
DEPENDPATH += """"". ../core ../cscope"""""

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
HEADERS += queryview.h \
    strings.h \
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
    buildprogress.h \
    version.h
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
    $${QSCI_ROOT_PATH}/include/Qsci \
    .
LIBS += -L../core \
    -lkscope_core \
    -L../cscope \
    -lkscope_cscope \
    -L$${QSCI_ROOT_PATH}/lib \
    -lqscintilla2
RESOURCES = ide.qrc
target.path = $${INSTALL_PATH}/bin
INSTALLS += target
