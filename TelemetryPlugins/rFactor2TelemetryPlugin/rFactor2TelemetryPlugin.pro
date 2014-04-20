#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T18:49:30
#
#-------------------------------------------------

QT += core gui widgets

TARGET = rFactor2TelemetryPlugin

#CONFIG += staticlib
TEMPLATE      = lib
 CONFIG       += plugin
 INCLUDEPATH  += ../../OpenSimwheelCommander/
 HEADERS       = rFactor2TelemetryPlugin.h \
    TelemetryInfo.h \
    SettingsGroupBox.h \
    PluginSettings.h
 SOURCES       = rFactor2TelemetryPlugin.cpp \
    SettingsGroupBox.cpp \
    PluginSettings.cpp
 TARGET        = $$qtLibraryTarget(osw_plugin_rfactor2)
 DESTDIR       = ../plugins

 # install
 target.path = ./plugins/target
 sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS
 sources.path = src
 INSTALLS += target sources

OTHER_FILES += \
    osw_plugin_rfactor2.json

FORMS += \
    SettingsGroupBox.ui


