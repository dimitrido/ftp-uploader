#ifndef FtpNetworkAccessManager_H
#define FtpNetworkAccessManager_H

#include <QObject>
#include <QUrl>
#include <QNetworkRequest>
#include <QIODevice>

class FtpNetworkReply;
class FtpNetworkAccessManagerPrivate;

class FtpNetworkAccessManager : public QObject
{
    Q_OBJECT

public:
    explicit FtpNetworkAccessManager(QObject *parent = nullptr);
    ~FtpNetworkAccessManager();

    // API similar to QNetworkAccessManager
    FtpNetworkReply* get(const QNetworkRequest &request);
    FtpNetworkReply* put(const QNetworkRequest &request, QIODevice *data);
    FtpNetworkReply* deleteResource(const QNetworkRequest &request);

    // FTP-specific methods
    void setUserName(const QString &userName);
    void setPassword(const QString &password);
    QString userName() const;
    QString password() const;
    
    // SSL/TLS configuration
    void setSslCertificateVerification(bool verify);
    bool sslCertificateVerification() const;

Q_SIGNALS:
    void finished(FtpNetworkReply *reply);

private:
    FtpNetworkAccessManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(FtpNetworkAccessManager)
    Q_DISABLE_COPY(FtpNetworkAccessManager)
};

#endif // FtpNetworkAccessManager_H
