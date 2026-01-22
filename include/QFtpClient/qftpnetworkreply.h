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
    enum NetworkError {
        NoError = 0,
        ConnectionRefusedError,
        RemoteHostClosedError,
        HostNotFoundError,
        TimeoutError,
        OperationCanceledError,
        SslHandshakeFailedError,
        TemporaryNetworkFailureError,
        NetworkSessionFailedError,
        BackgroundRequestNotAllowedError,
        UnknownNetworkError = 99,
        
        // Protocol errors
        ProtocolUnknownError = 301,
        ProtocolInvalidOperationError,
        AuthenticationRequiredError = 401,
        ContentAccessDenied = 403,
        ContentNotFoundError = 404,
        ContentOperationNotPermittedError = 405,
        ProtocolFailure = 499
    };
    Q_ENUM(NetworkError)

    enum Operation {
        GetOperation,
        PutOperation,
        DeleteOperation
    };

    ~QFtpNetworkReply();

    QUrl url() const;
    Operation operation() const;
    NetworkError error() const;
    QString errorString() const;
    bool isFinished() const;
    bool isRunning() const;
    QByteArray readAll();
    qint64 bytesAvailable() const;

    void abort();

Q_SIGNALS:
    void finished();
    void error(QFtpNetworkReply::NetworkError code);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    friend class QFtpNetworkAccessManager;
    friend class QFtpNetworkAccessManagerPrivate;
    
    explicit QFtpNetworkReply(QObject *parent = nullptr);
    
    QFtpNetworkReplyPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QFtpNetworkReply)
    Q_DISABLE_COPY(QFtpNetworkReply)
};

#endif // QFTPNETWORKREPLY_H
