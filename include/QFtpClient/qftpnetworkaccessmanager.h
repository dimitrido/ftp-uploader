#ifndef QFTPNETWORKACCESSMANAGER_H
#define QFTPNETWORKACCESSMANAGER_H

#include <QObject>
#include <QUrl>
#include <QNetworkRequest>

class QFtpNetworkReply;
class QFtpNetworkAccessManagerPrivate;

class QFtpNetworkAccessManager : public QObject
{
    Q_OBJECT

public:
    explicit QFtpNetworkAccessManager(QObject *parent = nullptr);
    ~QFtpNetworkAccessManager();

    // API similar to QNetworkAccessManager
    QFtpNetworkReply* get(const QNetworkRequest &request);
    QFtpNetworkReply* put(const QNetworkRequest &request, QIODevice *data);
    QFtpNetworkReply* deleteResource(const QNetworkRequest &request);

    // FTP-specific methods
    void setUserName(const QString &userName);
    void setPassword(const QString &password);
    QString userName() const;
    QString password() const;

Q_SIGNALS:
    void finished(QFtpNetworkReply *reply);

private:
    QFtpNetworkAccessManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QFtpNetworkAccessManager)
    Q_DISABLE_COPY(QFtpNetworkAccessManager)
};

#endif // QFTPNETWORKACCESSMANAGER_H
