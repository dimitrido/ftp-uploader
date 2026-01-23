#ifndef FtpNetworkReply_H
#define FtpNetworkReply_H

#include <QObject>
#include <QUrl>
#include <QByteArray>
#include <QNetworkRequest>

class FtpNetworkReplyPrivate;

class FtpNetworkReply : public QObject
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

    ~FtpNetworkReply();

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
    void error(FtpNetworkReply::NetworkError code);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    friend class FtpNetworkAccessManager;
    friend class FtpNetworkAccessManagerPrivate;
    
    explicit FtpNetworkReply(QObject *parent = nullptr);
    
    FtpNetworkReplyPrivate *d_ptr;
    Q_DECLARE_PRIVATE(FtpNetworkReply)
    Q_DISABLE_COPY(FtpNetworkReply)
};

#endif // FtpNetworkReply_H
