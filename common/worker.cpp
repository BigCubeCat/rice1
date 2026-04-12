#include "worker.hpp"
//
#include <QBuffer>

TNetworkWorker::TNetworkWorker(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this)) { }

void TNetworkWorker::sendRequest(const QString &url, const QString &xmlData) {
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");

    QNetworkReply *reply = m_manager->post(request, xmlData.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString response = reply->readAll();
            emit responseReceived(response);
        }
        else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void TNetworkWorker::sendPatchRequest(
    const QString &url, const QString &xmlData
) {
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");

    QBuffer *buffer = new QBuffer();    // NOLINT
    buffer->setData(xmlData.toUtf8());
    buffer->open(QIODevice::ReadOnly);

    // Отправляем PATCH-запрос. QNetworkAccessManager заберёт владение buffer и
    // удалит его после отправки.
    QNetworkReply *reply =
        m_manager->sendCustomRequest(request, "PATCH", buffer);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString response = reply->readAll();
            emit responseReceived(response);
        }
        else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void TNetworkWorker::getRequest(const QString &url) {
    QNetworkRequest request((QUrl(url)));
    QNetworkReply *reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString response = reply->readAll();
            emit responseReceived(response);
        }
        else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}
