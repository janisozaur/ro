#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T17:43:23
#
#-------------------------------------------------

QT       += core network gui

TARGET = classifier
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -fopenmp

include(../common/projectsCommon.pro)

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
    ../common/LabelledData.h \
    ../common/CommonDefines.h
