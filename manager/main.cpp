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


namespace {
void setupRouter(QHttpServer &server) {
    server.route("/", []() { return "Hello world"; });
    server.route(
        "/api/hash/crack",
        QHttpServerRequest::Method::Post,
        [](const QHttpServerRequest &request, QHttpServerResponder &responder) {
        }
    );
    server.route(
        "/api/hash/status",
        QHttpServerRequest::Method::Get,
        [](const QHttpServerRequest &request) {
            QUrl url = request.url();
            qDebug() << "url=" << url;
            QUrlQuery query(url);
            QString requestId = query.queryItemValue("requestId");
            qDebug() << "status: " << requestId;

            QJsonObject json;
            json["status"]  = "ok";
            json["message"] = "Hello world";
            QJsonDocument doc(json);
            qDebug() << "json=" << doc.toJson();
            return QHttpServerResponse(doc.toJson());
        }
    );
}
};    // namespace


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QHttpServer server;
    setupRouter(server);


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
