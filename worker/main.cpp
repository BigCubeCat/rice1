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

#include "processor.hpp"
#include "request.hxx"
#include "response.hxx"
#include "utils.hpp"


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QHttpServer server;

    auto *thread = new QThread();    // NOLINT
    TNetworkWorker worker;
    const auto url    = utils::getEnv("MANAGER_URL");
    QString urlString = url.has_value() ? url.value() : "";
    TProcessor processor(thread);
    processor.setUrls(urlsList.split(","));
    worker.moveToThread(thread);
    thread->start();


    thread->start();

    server.route("/", []() { return "Hello world"; });
    server.route(
        "/internal/api/manager/hash/crack/request",
        QHttpServerRequest::Method::Patch,
        [&](const QHttpServerRequest &request) {
            const auto body = request.body();
            processor.addTask(utils::body2worker(body));
            return "";
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
