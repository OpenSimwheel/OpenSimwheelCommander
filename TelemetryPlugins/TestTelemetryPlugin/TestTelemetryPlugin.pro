#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T18:49:30
#
#-------------------------------------------------



QT += core gui widgets



TARGET = TestTelemetryPlugin

#CONFIG += staticlib
TEMPLATE      = lib
 CONFIG       += plugin
 INCLUDEPATH  += ../../OpenSimwheelCommander/
 HEADERS       = TestTelemetryPlugin.h \
    TelemetryInfo.h \
    SettingsGroupBox.h \
    PluginSettings.h
 SOURCES       = TestTelemetryPlugin.cpp \
    SettingsGroupBox.cpp \
    PluginSettings.cpp
 TARGET        = $$qtLibraryTarget(osw_plugin_test)
 DESTDIR       = ../plugins

 # install
 target.path = ./plugins/target
 sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS
 sources.path = src
 INSTALLS += target sources

OTHER_FILES += \
    osw_plugin_test.json \

FORMS += \
    SettingsGroupBox.ui


