#include "PluginSettings.h"
#include "QDir"


PluginSettings::PluginSettings(QObject *parent) : QObject(parent)
{
    QDir settingsDir = QFileInfo(QSettings(QSettings::IniFormat, QSettings::UserScope, "OpenSimwheel", "OpenSimwheel Commander").fileName()).absoluteDir();

    if (!settingsDir.cd("Plugins")) {
        settingsDir.mkdir("Plugins");
        settingsDir.cd("Plugins");
    }

    QString settingsFilePath = settingsDir.absolutePath() + "\\osw_plugin_iracing.ini";

    settings = new QSettings(settingsFilePath, QSettings::IniFormat);

    Load();

    CarName = "";
}

void PluginSettings::Load(QString category) {
    QString key(category + "/");

    this->useHardstops = settings->value(key + "UseHardstops", true).toBool();
    this->smoothingPct = settings->value(key + "SmoothingPct", int(0)).toInt();
    this->enable = settings->value(key + "Enable", false).toBool();
}

void PluginSettings::Save(QString category) {
    QString key(category + "/");

    settings->setValue(key + "UseHardstops", useHardstops);
    settings->setValue(key + "SmoothingPct", smoothingPct);

    if (category != "General") {
        settings->setValue(key + "Enable", enable);
    }

    settings->sync();
}
