TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Encriptado.cpp \
        LZ78.cpp \
        RLE.cpp \
        main.cpp

HEADERS += \
    Encriptado.h \
    LZ78.h \
    RLE.h
