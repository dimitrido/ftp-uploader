#ifndef QFTPNETWORKACCESSMANAGER_H
#define QFTPNETWORKACCESSMANAGER_H

#include <QObject>
#include <QUrl>
#include <QNetworkRequest>
#include <QIODevice>

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
    // Private implementation pointer
    QFtpNetworkAccessManagerPrivate *d_ptr;
    
    // Helper methods for private data access
    inline QFtpNetworkAccessManagerPrivate* d_func() { return d_ptr; }
    inline const QFtpNetworkAccessManagerPrivate* d_func() const { return d_ptr; }
    
    // Non-copyable
    QFtpNetworkAccessManager(const QFtpNetworkAccessManager&) = delete;
    QFtpNetworkAccessManager& operator=(const QFtpNetworkAccessManager&) = delete;
};

#endif // QFTPNETWORKACCESSMANAGER_H
