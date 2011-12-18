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

QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -ftree-vectorizer-verbose=2

include(../common/projectsCommon.pro)

LIBS += -lgomp

SOURCES += main.cpp \
    ExtractorFactory.cpp \
    ExtractorInterface.cpp \
    extractors/FFT.cpp \
    extractors/Complex.cpp \
    ../common/LabelledData.cpp \
    extractors/LBP.cpp \
    extractors/AutoCorrelation.cpp \
    extractors/FFTSum.cpp \
    extractors/Moments.cpp \
    extractors/FFTMoments.cpp

HEADERS += \
    ExtractorFactory.h \
    ExtractorInterface.h \
    extractors/FFT.h \
    extractors/ComplexArray.h \
    extractors/Complex.h \
    ../common/TypeCommon.h \
    ../common/LabelledData.h \
    ../common/CommonDefines.h \
    extractors/LBP.h \
    extractors/AutoCorrelation.h \
    extractors/FFTSum.h \
    extractors/Moments.h \
    extractors/FFTMoments.h
