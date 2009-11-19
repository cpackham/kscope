include(../config)
TEMPLATE = app
TARGET = kscopeapp
DEPENDPATH += ". ../core ../cscope ../editor"

# Input
SOURCES += openprojectdialog.cpp \
    settings.cpp \
    session.cpp \
    projectmanager.cpp \
    projectdialog.cpp \
    projectfilesdialog.cpp \
    stackwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    actions.cpp \
    application.cpp \
    querydialog.cpp \
    editorcontainer.cpp \
    queryresultdock.cpp \
    queryresultdialog.cpp \
    addfilesdialog.cpp \
    configenginesdialog.cpp
HEADERS += openprojectdialog.h \
    settings.h \
    session.h \
    queryview.h \
    strings.h \
    locationhistory.h \
    projectmanager.h \
    projectfilesdialog.h \
    stackwidget.h \
    mainwindow.h \
    actions.h \
    application.h \
    querydialog.h \
    editorcontainer.h \
    queryresultdock.h \
    queryresultdialog.h \
    addfilesdialog.h \
    projectdialog.h \
    buildprogress.h \
    version.h \
    configenginesdialog.h
FORMS += querydialog.ui \
    queryresultdialog.ui \
    stackpage.ui \
    projectfilesdialog.ui \
    addfilesdialog.ui \
    projectdialog.ui \
    configenginesdialog.ui \
    openprojectdialog.ui
INCLUDEPATH += .. \
    $${QSCI_HEADER_PATH}/Qsci \
    .
LIBS += -L../core \
    -lkscope_core \
    -L../cscope \
    -lkscope_cscope \
    -L../editor \
    -lkscope_editor
RESOURCES = app.qrc
target.path = $${INSTALL_PATH}/bin
INSTALLS += target
