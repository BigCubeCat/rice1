#pragma once

#include <QObject>
#include <QSettings>


struct WorkerSettings {
    quint16 port = 8080;
};


inline WorkerSettings loadSettings(const QString &configFile) {
    WorkerSettings result;
    QSettings settings(configFile, QSettings::IniFormat);
    settings.beginGroup("server");
    settings.value("port", 8080).toUInt();
    settings.endGroup();
    return result;
}
