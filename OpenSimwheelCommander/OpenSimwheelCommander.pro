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
    libs/irsdk/irsdk_utils.cpp \
    libs/irsdk/yaml_parser.cpp \
    MainWindow.cpp \
    ForceFeedbackProcessor.cpp \
    TelemetryWorker.cpp \
    DriveWorker.cpp \
    Dialogs/DriveStageConfigDialog.cpp \
    Dialogs/WheelSettingsDialog.cpp \
    Dialogs/JoystickConfigurationDialog.cpp \
    Splash/IndestructableSplashScreen.cpp \
    SimpleMotionCommunicator.cpp \
    JoystickManager.cpp

HEADERS  += \
    libs/irsdk/irsdk_defines.h \
    libs/irsdk/yaml_parser.h \
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
    JoystickManager.h

FORMS    += \
    MainWindow.ui \
    Dialogs/DriveStageConfigDialog.ui \
    Dialogs/WheelSettingsDialog.ui \
    Dialogs/JoystickConfigurationDialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/vJoySDK/lib/ -lvJoyInterface

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
    Resources/OpenSimwheelCommander.ico \
    OpenSimwheelCommander.rc

RC_FILE = OpenSimwheelCommander.rc
