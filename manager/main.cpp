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
#include "utils.hpp"
#include "worker.hpp"


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QHttpServer server;

    auto *thread     = new QThread();    // NOLINT
    TManager manager = TManager();
    TNetworkWorker worker;
    const auto urlsList = utils::getEnv("WORKER_URLS");
    QString urlsListStr = urlsList.has_value() ? urlsList.value() : "";
    manager.setUrls(urlsListStr.split(","));
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
    server.route(
        "/internal/api/manager/hash/crack/request",
        QHttpServerRequest::Method::Patch,
        [&](const QHttpServerRequest &request) {
            return manager.internalHandler(request);
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
