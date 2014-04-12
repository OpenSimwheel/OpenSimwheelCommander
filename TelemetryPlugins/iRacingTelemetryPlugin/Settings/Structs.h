#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct
{
    bool useHardstops;
    int smoothingPct;
} PluginSettings;

Q_DECLARE_METATYPE(PluginSettings)

#endif // STRUCTS_H
