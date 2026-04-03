#include "worker.hpp"


TWorker::TWorker(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this)) { }

void TWorker::sendRequest(const QString &url, const QString &xmlData) {
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

void TWorker::getRequest(const QString &url) {
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
