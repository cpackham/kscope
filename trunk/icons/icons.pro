include(../config)
TEMPLATE = lib
TARGET = kscope_icons
CONFIG += dll

HEADERS += qticonloader.h
SOURCES += qticonloader.cpp

target.path = $${INSTALL_PATH}/lib
INSTALLS += target
