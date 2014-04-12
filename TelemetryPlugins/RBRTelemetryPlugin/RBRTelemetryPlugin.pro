#-------------------------------------------------
#
# Project created by QtCreator 2014-03-15T18:22:01
#
#-------------------------------------------------

TARGET = RBRTelemetryPlugin

QT += core gui

#CONFIG += staticlib
TEMPLATE      = lib
 CONFIG       += plugin
 INCLUDEPATH  += ../../OpenSimwheelCommander/
 SOURCES += RBRTelemetryPlugin.cpp
 HEADERS += RBRTelemetryPlugin.h
 TARGET        = $$qtLibraryTarget(osw_plugin_rbr)
 DESTDIR       = ../plugins

 # install
 target.path = ./plugins/target
 sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS
 sources.path = src
 INSTALLS += target sources

OTHER_FILES += \
    osw_plugin_rbr.json \
    osw_plugin_rbr.json

