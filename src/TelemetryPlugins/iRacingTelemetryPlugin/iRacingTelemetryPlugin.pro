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

TARGET = iRacingTelemetryPlugin


 INCLUDEPATH  += ../../OpenSimwheelCommander/
 HEADERS       = iRacingTelemetryPlugin.h \
    irsdk/irsdk_defines.h \
    irsdk/yaml_parser.h \
    Settings/SettingsGroupBox.h \
    Settings/PluginSettings.h
 SOURCES       = iRacingTelemetryPlugin.cpp \
    irsdk/irsdk_utils.cpp \
    irsdk/yaml_parser.cpp \
    Settings/SettingsGroupBox.cpp \
    Settings/PluginSettings.cpp
 TARGET        = $$qtLibraryTarget(osw_plugin_iracing)

OTHER_FILES += \
    osw_plugin_iracing.json

FORMS += \
    Settings/settingsgroupbox.ui

