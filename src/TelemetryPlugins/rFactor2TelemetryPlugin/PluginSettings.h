#ifndef PLUGINSETTINGS_H
#define PLUGINSETTINGS_H

#include <QSettings>
#include <QObject>

class PluginSettings : public QObject {
    Q_OBJECT

public:
    explicit PluginSettings(QObject *parent = 0);
//    ~PluginSettings();

//    void SetUseHardstops(bool value) { useHardstops = value; }
    void SetEnable(bool value) { enable = value; }
    void SetSmoothingPct(int pct) { smoothingPct = pct; }
    void SetMaxSteeringArmForce(int value) { maxSteeringArmForce = value; }
    void SetIsInverted(bool value) { invert = value; }

//    bool GetUseHardstops() { return useHardstops; }
    int GetSmoothingPct() { return smoothingPct; }
    bool GetEnable() { return enable; }
    int GetMaxSteeringArmForce() { return maxSteeringArmForce; }
    bool GetIsInverted() { return invert; }

    void Load(QString category = "General");
    void Save(QString category = "General");

    QString CarName;

private:

    bool useHardstops;
    int smoothingPct;
    int maxSteeringArmForce;
    bool enable;
    bool invert;

    QSettings* settings;
};

#endif // PLUGINSETTINGS_H
