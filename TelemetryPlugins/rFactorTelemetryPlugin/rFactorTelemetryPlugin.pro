#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T18:49:30
#
#-------------------------------------------------

QT += core gui widgets

TARGET = rFactorTelemetryPlugin

#CONFIG += staticlib
TEMPLATE      = lib
 CONFIG       += plugin
 INCLUDEPATH  += ../../OpenSimwheelCommander/
 HEADERS       = rFactorTelemetryPlugin.h \
    TelemetryInfo.h \
    SettingsGroupBox.h \
    PluginSettings.h
 SOURCES       = rFactorTelemetryPlugin.cpp \
    SettingsGroupBox.cpp \
    PluginSettings.cpp
 TARGET        = $$qtLibraryTarget(osw_plugin_rfactor)
 DESTDIR       = ../plugins

 # install
 target.path = ./plugins/target
 sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS
 sources.path = src
 INSTALLS += target sources

OTHER_FILES += \
    osw_plugin_rfactor.json

FORMS += \
    SettingsGroupBox.ui


