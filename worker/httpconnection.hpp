#pragma once

#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QXmlStreamReader>

class THttpConnection : public QObject {
    Q_OBJECT
public:
    explicit THttpConnection(QTcpSocket *sock, QObject *parent = nullptr)
        : QObject(parent), m_socket(sock) {
        connect(
            m_socket,
            &QTcpSocket::readyRead,
            this,
            &THttpConnection::onReadyRead
        );
        connect(
            m_socket,
            &QTcpSocket::disconnected,
            this,
            &THttpConnection::onDisconnected
        );
    }

private slots:
    void onReadyRead() {
        m_buffer.append(m_socket->readAll());

        int idx = static_cast<int>(m_buffer.indexOf("\r\n\r\n"));
        if (idx == -1)
            return;    // ещё не прочитали заголовки полностью

        QByteArray headerPart = m_buffer.left(idx);
        QByteArray bodyPart   = m_buffer.mid(idx + 4);

        // Разбор стартовой строки и заголовков (простейший)
        QList<QByteArray> lines = headerPart.split('\n');
        if (lines.isEmpty()) {
            sendResponse(400, "Bad Request");
            return;
        }

        // request line: METHOD PATH HTTP/1.1
        QByteArray requestLine  = lines.takeFirst().trimmed();
        QList<QByteArray> parts = requestLine.split(' ');
        if (parts.size() < 3) {
            sendResponse(400, "Bad Request");
            return;
        }
        QString method = QString::fromUtf8(parts.at(0));
        QString path   = QString::fromUtf8(parts.at(1));

        // Парсим заголовки
        QHash<QByteArray, QByteArray> headers;
        for (auto &ln : lines) {
            auto s = ln.trimmed();
            if (s.isEmpty())
                continue;
            int colon = static_cast<int>(s.indexOf(':'));
            if (colon > 0) {
                QByteArray name = s.left(colon).trimmed().toLower();
                QByteArray val  = s.mid(colon + 1).trimmed();
                headers.insert(name, val);
            }
        }

        // Требуем POST
        if (method != "POST") {
            sendResponse(405, "Method Not Allowed");
            return;
        }

        // Проверяем Content-Length
        qint64 contentLength = 0;
        if (headers.contains("content-length")) {
            bool ok       = false;
            contentLength = headers.value("content-length").toLongLong(&ok);
            if (!ok) {
                sendResponse(411, "Length Required");
                return;
            }
        }
        else {
            sendResponse(411, "Length Required");
            return;
        }

        // Ждём пока тело полностью придёт
        if (bodyPart.size() < contentLength) {
            // ещё не все байты пришли
            return;
        }

        QByteArray xmlBody = bodyPart.left(contentLength);

        // Обработка XML (потоковый парсер)
        QString parseResult = handleXml(xmlBody);

        // Отправляем ответ
        QByteArray respBody = parseResult.toUtf8();
        QByteArray resp     = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: text/plain; charset=utf-8\r\n"
                              "Content-Length: "
                          + QByteArray::number(respBody.size())
                          + "\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                          + respBody;

        m_socket->write(resp);
        m_socket->disconnectFromHost();
    }

    void onDisconnected() {
        m_socket->deleteLater();
        deleteLater();
    }

public:
private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;

    // Простейшая обработка XML: демонстрация чтения элементов и значений
    QString handleXml(const QByteArray &xml) {
        QXmlStreamReader reader(xml);
        QStringList elementsFound;
        // Пример: находим все элементы <name>...</name> и собираем текст
        while (!reader.atEnd()) {
            reader.readNext();
            if (reader.isStartElement()) {
                auto name = reader.name().toString();
                if (name == "name") {
                    QString text = reader.readElementText();
                    elementsFound << text;
                }
                else {
                    // Можно обрабатывать другие теги по необходимости
                }
            }
        }

        if (reader.hasError()) {
            return QString("XML parse error: %1").arg(reader.errorString());
        }

        if (elementsFound.isEmpty())
            return "OK: received XML, no <name> tags found.";
        return QString("OK: names: %1").arg(elementsFound.join(", "));
    }

    void sendResponse(int code, const QString &message) {
        QByteArray body = message.toUtf8();
        QByteArray resp = "HTTP/1.1 " + QByteArray::number(code) + " "
                          + message.toUtf8()
                          + "\r\n"
                            "Content-Type: text/plain; charset=utf-8\r\n"
                            "Content-Length: "
                          + QByteArray::number(body.size())
                          + "\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                          + body;
        m_socket->write(resp);
        m_socket->disconnectFromHost();
    }
};
