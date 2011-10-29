#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T17:43:23
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = classifier
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -mtune=core2 -march=core2 -O2

LIBS += -lgomp

SOURCES += main.cpp \
    ClassifierInterface.cpp \
    FeatureImporter.cpp \
    CpuClassifier.cpp \
    SortItem.cpp

HEADERS += \
    ClassifierInterface.h \
    FeatureImporter.h \
    CpuClassifier.h \
    SortItem.h








