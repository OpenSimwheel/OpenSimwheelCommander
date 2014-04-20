#-------------------------------------------------
#
# Project created by QtCreator 2014-01-06T15:31:10
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

include(libs/SimpleMotionV2/SimpleMotionV2.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# remove possible other optimization flags
#QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O3

# add the desired -O2 if not present
#QMAKE_CXXFLAGS_RELEASE *= -O2

TARGET = OpenSimwheelCommander
TEMPLATE = app


SOURCES += main.cpp\
    MainWindow.cpp \
    ForceFeedbackProcessor.cpp \
    TelemetryWorker.cpp \
    DriveWorker.cpp \
    Dialogs/DriveStageConfigDialog.cpp \
    Dialogs/WheelSettingsDialog.cpp \
    Dialogs/JoystickConfigurationDialog.cpp \
    Splash/IndestructableSplashScreen.cpp \
    SimpleMotionCommunicator.cpp \
    JoystickManager.cpp \
    Dialogs/PluginDialog.cpp \
    Dialogs/OptionsDialog.cpp

HEADERS  += \
    CommonStructs.h \
    MainWindow.h \
    ForceFeedbackProcessor.h \
    TelemetryWorker.h \
    DriveWorker.h \
    Dialogs/DriveStageConfigDialog.h \
    Dialogs/WheelSettingsDialog.h \
    Version.h \
    Dialogs/JoystickConfigurationDialog.h \
    Splash/IndestructableSplashScreen.h \
    SimpleMotionCommunicator.h \
    JoystickManager.h \
    TelemetryPlugins/TelemetryPluginInterface.h \
    Dialogs/PluginDialog.h \
    Dialogs/OptionsDialog.h \
    TelemetryPlugins/NullTelemetryPlugin.h

FORMS    += \
    MainWindow.ui \
    Dialogs/DriveStageConfigDialog.ui \
    Dialogs/WheelSettingsDialog.ui \
    Dialogs/JoystickConfigurationDialog.ui \
    Dialogs/OptionsDialog.ui




win32 {
    DEFINES += VERSION_BUILD_TIME=\\\"$$system('echo %time%')\\\"
    DEFINES += VERSION_BUILD_DATE=\\\"$$system('echo %date%')\\\"
} else {
    DEFINES += VERSION_BUILD_TIME=\\\"$$system(date '+%H:%M.%s')\\\"
    DEFINES += VERSION_BUILD_DATE=\\\"$$system(date '+%d/%m/%y')\\\"
}

RESOURCES += \
    Resources/Images.qrc

OTHER_FILES += \
    OpenSimwheelCommander.rc

RC_FILE = OpenSimwheelCommander.rc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/vJoySDK/lib/ -lvJoyInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/vJoySDK/lib/ -lvJoyInterface

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_iracing
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_iracingd

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_rbr
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_rbrd

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_rfactor
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_rfactord

##win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_rfactor2
##else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_rfactor2d

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_test
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_testd

##win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_assettocorsa
##else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../TelemetryPlugins/plugins/ -losw_plugin_assettocorsad



INCLUDEPATH += $$PWD/../TelemetryPlugins/plugins
DEPENDPATH += $$PWD/../TelemetryPlugins/plugins

DESTDIR = ../bin

#install plugins
plugins_to_install.path = $$DESTDIR/plugins/
plugins_to_install.files += $$PWD/../TelemetryPlugins/plugins/*.dll
INSTALLS += plugins_to_install

#install core-dependencies
qt_dependencies.path = $$DESTDIR
qt_dependencies.files = $$PWD/../qt-bin/*
INSTALLS += qt_dependencies
