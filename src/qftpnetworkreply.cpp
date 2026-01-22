#include "QFtpClient/qftpnetworkreply.h"
#include "private/qftpnetworkreply_p.h"

QFtpNetworkReply::QFtpNetworkReply(QObject *parent)
    : QObject(parent), d_ptr(new QFtpNetworkReplyPrivate())
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
    return d->running.loadRelaxed() != 0;
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
        // Set running to false to signal the progress callback to abort
        d->running.storeRelaxed(0);
    }
}
