#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QXmlStreamReader>

#include "frontserver.hpp"
#include "settings.hpp"


int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QString configFile = "config.ini";
    if (argc == 2) {
        configFile = argv[1];
    }
    WorkerSettings settings = loadSettings(configFile);

    TFrontServer server;
    if (!server.listen(QHostAddress::AnyIPv4, settings.port)) {
        qFatal("Cannot start server: %s", qPrintable(server.errorString()));
        return 1;
    }
    qInfo() << "HTTP server listening on port" << settings.port;
    return QCoreApplication::exec();
}
