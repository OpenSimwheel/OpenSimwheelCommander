#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T18:49:30
#
#-------------------------------------------------

QT += core gui

TARGET = iRacingTelemetryPlugin

#CONFIG += staticlib
TEMPLATE      = lib
 CONFIG       += plugin
 INCLUDEPATH  += ../../
 HEADERS       = iRacingTelemetryPlugin.h \
    irsdk/irsdk_defines.h \
    irsdk/yaml_parser.h
 SOURCES       = iRacingTelemetryPlugin.cpp \
    irsdk/irsdk_utils.cpp \
    irsdk/yaml_parser.cpp
 TARGET        = $$qtLibraryTarget(osw_plugin_iracing)
 DESTDIR       = ../plugins

#EXAMPLE_FILES = iracingplugin.json

 # install
 target.path = ./plugins/target
 sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS
 sources.path = src
 INSTALLS += target sources

OTHER_FILES += \
    osw_plugin_iracing.json

