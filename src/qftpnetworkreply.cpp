#include "QFtpClient/qftpnetworkreply.h"

// Forward declaration
class QFtpNetworkReplyPrivate;

QFtpNetworkReply::QFtpNetworkReply(QObject *parent)
    : QObject(parent), d_ptr(nullptr)
{
}

QFtpNetworkReply::~QFtpNetworkReply()
{
    if (d_ptr) {
        delete d_ptr;
    }
}

QUrl QFtpNetworkReply::url() const
{
    Q_D(const QFtpNetworkReply);
    return d->url;
}

QFtpNetworkReply::Operation QFtpNetworkReply::operation() const
{
    Q_D(const QFtpNetworkReply);
    return d->operation;
}

QFtpNetworkReply::NetworkError QFtpNetworkReply::error() const
{
    Q_D(const QFtpNetworkReply);
    return d->errorCode;
}

QString QFtpNetworkReply::errorString() const
{
    Q_D(const QFtpNetworkReply);
    return d->errorString;
}

bool QFtpNetworkReply::isFinished() const
{
    Q_D(const QFtpNetworkReply);
    return d->finished;
}

bool QFtpNetworkReply::isRunning() const
{
    Q_D(const QFtpNetworkReply);
    return d->running;
}

QByteArray QFtpNetworkReply::readAll()
{
    Q_D(QFtpNetworkReply);
    QByteArray data = d->buffer;
    d->buffer.clear();
    return data;
}

qint64 QFtpNetworkReply::bytesAvailable() const
{
    Q_D(const QFtpNetworkReply);
    return d->buffer.size();
}

void QFtpNetworkReply::abort()
{
    Q_D(QFtpNetworkReply);
    if (d->curl) {
        // Note: curl_easy_pause might not work as expected in all scenarios
        // A more robust solution would involve a cancellation flag
        d->running = false;
    }
}
