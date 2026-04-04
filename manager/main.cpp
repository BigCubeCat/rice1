#include <QCoreApplication>
#include <QDebug>
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QString>
#include <QTcpServer>
#include <QTimer>
#include <QUuid>

#include <memory.h>

#include "manager.hpp"
#include "worker.hpp"

QString getEnv(const std::string &name) {
    const char *val = std::getenv(name.c_str());
    return val ? QString(val) : "";
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QHttpServer server;

    auto *thread     = new QThread();    // NOLINT
    TManager manager = TManager();
    TWorker worker   = TWorker();
    QString urlsList = getEnv("WORKER_URLS");
    manager.setUrls(urlsList.split(","));
    worker.moveToThread(thread);
    thread->start();

    server.route("/", []() { return "Hello world"; });
    server.route(
        "/api/hash/crack",
        QHttpServerRequest::Method::Post,
        [&](const QHttpServerRequest &request) {
            return manager.crackHandler(request);
        }
    );
    server.route(
        "/api/hash/status",
        QHttpServerRequest::Method::Get,
        [&](const QHttpServerRequest &request) {
            return manager.statusHandler(request);
        }
    );

    auto tcpserver = std::make_unique<QTcpServer>();
    if (!tcpserver->listen(QHostAddress::Any, 8080)
        || !server.bind(tcpserver.get())) {
        qWarning() << QCoreApplication::translate(
            "QHttpServerExample", "Server failed to listen on a port."
        );
        return -1;
    }
    auto port = tcpserver->serverPort();

    qInfo().noquote() << QCoreApplication::translate(
                             "QHttpServerExample",
                             "Running on http://127.0.0.1:%1/"
                             "(Press CTRL+C to quit)"
    )
                             .arg(port);

    return QCoreApplication::exec();
}
