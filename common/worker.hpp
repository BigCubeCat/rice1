#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QUrl>

class TNetworkWorker : public QObject {
    Q_OBJECT
public:
    explicit TNetworkWorker(QObject *parent = nullptr);

private:
public slots:
    void sendRequest(const QString &url, const QString &xmlData);
    void sendPatchRequest(const QString &url, const QString &xmlData);
    void getRequest(const QString &url);
signals:
    void responseReceived(const QString &xml);
    void errorOccurred(const QString &error);

private:
    QNetworkAccessManager *m_manager;
};
