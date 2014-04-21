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

TARGET = ACTelemetryPlugin

HEADERS       = ACTelemetryPlugin.h \
    TelemetryInfo.h \
    SettingsGroupBox.h \
    PluginSettings.h
SOURCES       = ACTelemetryPlugin.cpp \
    SettingsGroupBox.cpp \
    PluginSettings.cpp

TARGET        = $$qtLibraryTarget(osw_plugin_ac)


OTHER_FILES += \
    osw_plugin_ac.json \

FORMS += \
    SettingsGroupBox.ui


