#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T18:49:30
#
#-------------------------------------------------

# general stuff

QT += core gui widgets

ROOT = $$PWD/../../../

TEMPLATE      = lib
 CONFIG       += plugin

INCLUDEPATH  += $$ROOT/src/TelemetryPluginInterface/inc/TelemetryPluginInterface.h
DESTDIR       = $$ROOT/bin/plugins/


# plugin specific stuff

TARGET = rFactor2TelemetryPlugin

 HEADERS       = rFactor2TelemetryPlugin.h \
    TelemetryInfo.h \
    SettingsGroupBox.h \
    PluginSettings.h
 SOURCES       = rFactor2TelemetryPlugin.cpp \
    SettingsGroupBox.cpp \
    PluginSettings.cpp
 TARGET        = $$qtLibraryTarget(osw_plugin_rfactor2)



OTHER_FILES += \
    osw_plugin_rfactor2.json

FORMS += \
    SettingsGroupBox.ui


