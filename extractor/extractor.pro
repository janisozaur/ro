#-------------------------------------------------
#
# Project created by QtCreator 2011-12-04T16:09:33
#
#-------------------------------------------------

QT       += core gui

TARGET = extractor
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -mtune=core2 -march=core2 -O3 -msse -msse2 -mssse3 -mmmx
QMAKE_CXXFLAGS += -ftree-vectorizer-verbose=2

LIBS += -lgomp

SOURCES += main.cpp \
    ExtractorFactory.cpp \
    ExtractorInterface.cpp \
    extractors/FFT.cpp \
    extractors/Complex.cpp \
    ../common/LabelledData.cpp

HEADERS += \
    ExtractorFactory.h \
    ExtractorInterface.h \
    extractors/FFT.h \
    extractors/ComplexArray.h \
    extractors/Complex.h \
    ../common/TypeCommon.h \
    ../common/LabelledData.h \
    ../common/CommonDefines.h
