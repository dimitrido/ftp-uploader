#ifndef QFTPNETWORKACCESSMANAGER_H
#define QFTPNETWORKACCESSMANAGER_H

#include <QObject>
#include <QUrl>
#include <QNetworkRequest>
#include <QIODevice>
#include <memory>

class QFtpNetworkReply;
class QFtpNetworkAccessManagerPrivate;

class QFtpNetworkAccessManager : public QObject
{
    Q_OBJECT

public:
    explicit QFtpNetworkAccessManager(QObject *parent = nullptr);
    ~QFtpNetworkAccessManager();

    // FTP operations
    QFtpNetworkReply* get(const QNetworkRequest &request);
    QFtpNetworkReply* put(const QNetworkRequest &request, QIODevice *data);
    QFtpNetworkReply* deleteResource(const QNetworkRequest &request);

    // FTP-specific methods
    void setUserName(const QString &userName);
    void setPassword(const QString &password);
    QString userName() const;
    QString password() const;
    
    // SSL/TLS configuration
    void setSslCertificateVerification(bool verify);
    bool sslCertificateVerification() const;

Q_SIGNALS:
    void finished(QFtpNetworkReply *reply);

private:
    // Implementation using unique_ptr for automatic memory management
    std::unique_ptr<QFtpNetworkAccessManagerPrivate> m_impl;
    
    // Non-copyable
    QFtpNetworkAccessManager(const QFtpNetworkAccessManager&) = delete;
    QFtpNetworkAccessManager& operator=(const QFtpNetworkAccessManager&) = delete;
};

#endif // QFTPNETWORKACCESSMANAGER_H
