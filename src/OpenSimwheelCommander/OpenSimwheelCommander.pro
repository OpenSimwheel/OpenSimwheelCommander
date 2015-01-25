#-------------------------------------------------
#
# Project created by QtCreator 2014-01-06T15:31:10
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

#include(libs/SimpleMotionV2/SimpleMotionV2.pri)

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
    OpenSimwheelCommander.rc \
    Changelog.txt

RC_FILE = OpenSimwheelCommander.rc



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Libraries/vJoy/ -lvJoyInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Libraries/vJoy/ -lvJoyInterface

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Libraries/SimpleMotionV2/ -lSimpleMotionV2_OpenSimwheel
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Libraries/SimpleMotionV2/ -lSimpleMotionV2_OpenSimwheel


INCLUDEPATH += $$PWD/../TelemetryPlugins/plugins
DEPENDPATH += $$PWD/../TelemetryPlugins/plugins



ROOT = $$PWD/../../

DESTDIR = $$ROOT/bin/commander




DEPLOY_PATH = $$ROOT/deploy

#install core-dependencies
qt_dependencies.path = $$DEPLOY_PATH/
qt_dependencies.files = $$ROOT/res/qt-bin/*
INSTALLS += qt_dependencies

#install extra files
install_extrafiles.path = $$DEPLOY_PATH/
install_extrafiles.files = $$ROOT/src/OpenSimwheelCommander/Changelog.txt
INSTALLS += install_extrafiles

#install app
install_app.path = $$DEPLOY_PATH/
install_app.files = $$ROOT/bin/commander/*
INSTALLS += install_app

#install plugins
plugins_to_install.path = $$DEPLOY_PATH/plugins/
plugins_to_install.files += $$ROOT/bin/plugins/*.dll
INSTALLS += plugins_to_install

#install resources
resources_to_install.path = $$DEPLOY_PATH/resources/plugins/
resources_to_install.files += $$ROOT/res/plugins/*
INSTALLS += resources_to_install


#copy plugins to bin folder
copyplugin_to_bin.path = $$ROOT/bin/commander/plugins/
copyplugin_to_bin.files += $$ROOT/bin/plugins/*
INSTALLS += copyplugin_to_bin
