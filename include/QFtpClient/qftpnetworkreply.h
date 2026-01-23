#ifndef QFTPNETWORKREPLY_H
#define QFTPNETWORKREPLY_H

#include <QObject>
#include <QUrl>
#include <QByteArray>
#include <QNetworkRequest>

class QFtpNetworkReplyPrivate;

class QFtpNetworkReply : public QObject
{
    Q_OBJECT

public:
    // FTP-specific error codes
    enum FtpError {
        FtpNoError = 0,
        FtpConnectionFailed,
        FtpConnectionClosed,
        FtpHostUnavailable,
        FtpTimeout,
        FtpOperationAborted,
        FtpSslError,
        FtpNetworkFailure,
        FtpSessionError,
        FtpRequestBlocked,
        FtpUnknownError = 99,
        
        // FTP protocol errors
        FtpProtocolError = 301,
        FtpInvalidOperation,
        FtpAuthenticationFailed = 401,
        FtpAccessDenied = 403,
        FtpResourceNotFound = 404,
        FtpOperationNotAllowed = 405,
        FtpTransferFailed = 499
    };
    Q_ENUM(FtpError)

    enum FtpOperation {
        RetrieveOperation,
        StoreOperation,
        RemoveOperation
    };

    ~QFtpNetworkReply();

    QUrl url() const;
    FtpOperation operation() const;
    FtpError error() const;
    QString errorString() const;
    bool isFinished() const;
    bool isRunning() const;
    QByteArray readAll();
    qint64 bytesAvailable() const;

    void abort();

Q_SIGNALS:
    void finished();
    void error(QFtpNetworkReply::FtpError code);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    friend class QFtpNetworkAccessManager;
    friend class QFtpNetworkAccessManagerPrivate;
    friend class QFtpNetworkReplyPrivate;
    
    explicit QFtpNetworkReply(QObject *parent = nullptr);
    
    // Private implementation pointer
    QFtpNetworkReplyPrivate *d_ptr;
    
    // Helper methods for private data access
    inline QFtpNetworkReplyPrivate* d_func() { return d_ptr; }
    inline const QFtpNetworkReplyPrivate* d_func() const { return d_ptr; }
    
    // Non-copyable
    QFtpNetworkReply(const QFtpNetworkReply&) = delete;
    QFtpNetworkReply& operator=(const QFtpNetworkReply&) = delete;
};

#endif // QFTPNETWORKREPLY_H
