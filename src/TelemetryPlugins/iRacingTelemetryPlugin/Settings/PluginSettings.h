#ifndef PLUGINSETTINGS_H
#define PLUGINSETTINGS_H

#include <QSettings>
#include <QObject>

class PluginSettings : public QObject {
    Q_OBJECT

public:
    explicit PluginSettings(QObject *parent = 0);
//    ~PluginSettings();

    void SetUseHardstops(bool value) { useHardstops = value; }
    void SetEnable(bool value) { enable = value; }
    void SetSmoothingPct(int pct) { smoothingPct = pct; }

    bool GetUseHardstops() { return useHardstops; }
    int GetSmoothingPct() { return smoothingPct; }
    int GetEnable() { return enable; }

    void Load(QString category = "General");
    void Save(QString category = "General");

    QString CarName;

private:

    bool useHardstops;
    int smoothingPct;
    bool enable;

    QSettings* settings;
};

#endif // PLUGINSETTINGS_H
