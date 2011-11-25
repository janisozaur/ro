#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T17:43:23
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = classifier
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -mtune=core2 -march=core2 -O3 -ftree-vectorizer-verbose=2 -msse -msse2 -mssse3 -mmmx

LIBS += -lgomp

SOURCES += main.cpp \
    ClassifierInterface.cpp \
    FeatureImporter.cpp \
    SortItem.cpp \
    NaiveClassifier.cpp \
    SortingQueue.cpp

HEADERS += \
    ClassifierInterface.h \
    FeatureImporter.h \
    SortItem.h \
    NaiveClassifier.h \
    SortingQueue.h
