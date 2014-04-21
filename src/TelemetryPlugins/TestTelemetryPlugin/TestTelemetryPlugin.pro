#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T18:49:30
#
#-------------------------------------------------

# general stuff

QT += core gui widgets

ROOT = ../../../../

TEMPLATE      = lib
 CONFIG       += plugin

INCLUDEPATH  += $$ROOT/src/TelemetryPluginInterface/inc/TelemetryPluginInterface.h
DESTDIR       = $$ROOT/bin/plugins/


# plugin specific stuff

TARGET = TestTelemetryPlugin


 HEADERS       = TestTelemetryPlugin.h \
    TelemetryInfo.h \
    SettingsGroupBox.h \
    PluginSettings.h
 SOURCES       = TestTelemetryPlugin.cpp \
    SettingsGroupBox.cpp \
    PluginSettings.cpp
 TARGET        = $$qtLibraryTarget(osw_plugin_test)
 DESTDIR       = ../plugins


OTHER_FILES += \
    osw_plugin_test.json \

FORMS += \
    SettingsGroupBox.ui


