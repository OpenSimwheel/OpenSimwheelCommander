#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T18:49:30
#
#-------------------------------------------------



QT += core gui widgets



TARGET = ACTelemetryPlugin

#CONFIG += staticlib
TEMPLATE      = lib
 CONFIG       += plugin
 INCLUDEPATH  += ../../OpenSimwheelCommander/
 HEADERS       = ACTelemetryPlugin.h \
    TelemetryInfo.h \
    SettingsGroupBox.h \
    PluginSettings.h
 SOURCES       = ACTelemetryPlugin.cpp \
    SettingsGroupBox.cpp \
    PluginSettings.cpp
 TARGET        = $$qtLibraryTarget(osw_plugin_ac)
 DESTDIR       = ../plugins

 # install
 target.path = ./plugins/target
 sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS
 sources.path = src
 INSTALLS += target sources

OTHER_FILES += \
    osw_plugin_ac.json \

FORMS += \
    SettingsGroupBox.ui


