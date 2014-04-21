#-------------------------------------------------
#
# Project created by QtCreator 2014-03-15T18:22:01
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


TARGET = RBRTelemetryPlugin

 INCLUDEPATH  += ../../OpenSimwheelCommander/
 SOURCES += RBRTelemetryPlugin.cpp
 HEADERS += RBRTelemetryPlugin.h
 TARGET        = $$qtLibraryTarget(osw_plugin_rbr)


OTHER_FILES += \
    osw_plugin_rbr.json \
    osw_plugin_rbr.json

