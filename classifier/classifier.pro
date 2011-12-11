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
QMAKE_CXXFLAGS += -mtune=core2 -march=core2 -O3 -msse -msse2 -mssse3 -mmmx
#QMAKE_CXXFLAGS += -ftree-vectorizer-verbose=2

LIBS += -lgomp

SOURCES += main.cpp \
    ClassifierInterface.cpp \
    FeatureImporter.cpp \
    SortItem.cpp \
    KnnClassifier.cpp \
    SortingQueue.cpp \
    NeuralNetwork/Neuron.cpp \
    NeuralNetwork/Connection.cpp \
    NeuralNetwork/NetworkLayer.cpp \
    NeuralNetwork/NeuralNetwork.cpp \
    ../common/LabelledData.cpp

HEADERS += \
    ClassifierInterface.h \
    FeatureImporter.h \
    SortItem.h \
    KnnClassifier.h \
    SortingQueue.h \
    ../common/TypeCommon.h \
    NeuralNetwork/Neuron.h \
    NeuralNetwork/Connection.h \
    NeuralNetwork/NetworkLayer.h \
    NeuralNetwork/NeuralNetwork.h \
    ../common/LabelledData.h
