#-------------------------------------------------
#
# Project created by QtCreator 2011-12-12T04:12:20
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = trainer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../classifier/FeatureImporter.cpp \
    ../classifier/ClassifierInterface.cpp \
    ../classifier/NeuralNetwork/Neuron.cpp \
    ../classifier/NeuralNetwork/Connection.cpp \
    ../classifier/NeuralNetwork/NetworkLayer.cpp \
    ../classifier/NeuralNetwork/NeuralNetwork.cpp \
    ../common/LabelledData.cpp

HEADERS += \
    ../classifier/FeatureImporter.h \
    ../classifier/ClassifierInterface.h \
    ../common/TypeCommon.h \
    ../classifier/NeuralNetwork/Neuron.h \
    ../classifier/NeuralNetwork/Connection.h \
    ../classifier/NeuralNetwork/NetworkLayer.h \
    ../classifier/NeuralNetwork/NeuralNetwork.h \
    ../common/LabelledData.h
