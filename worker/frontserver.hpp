#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "httpconnection.hpp"

class TFrontServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TFrontServer(QObject *parent = nullptr) : QTcpServer(parent) { }

protected:
    void incomingConnection(qintptr handle) override {
        auto *sock = new QTcpSocket(this);    // NOLINT: QObject with parent
        if (!sock->setSocketDescriptor(handle)) {
            qWarning() << "Failed to set socket descriptor";
            sock->deleteLater();
            return;
        }
        // Переносим обработку соединения на отдельный QObject (внутри main
        // thread — для простоты). Для высокой нагрузки — создавайте отдельные
        // потоки или используйте пул потоков.
        new THttpConnection(sock, this);    // объект self-deletes при
                                            // отключении
    }
};
