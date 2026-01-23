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
    const QFtpNetworkReplyPrivate *d = d_func();
    return d->url;
}

QFtpNetworkReply::FtpOperation QFtpNetworkReply::operation() const
{
    const QFtpNetworkReplyPrivate *d = d_func();
    return d->operation;
}

QFtpNetworkReply::FtpError QFtpNetworkReply::error() const
{
    const QFtpNetworkReplyPrivate *d = d_func();
    return d->errorCode;
}

QString QFtpNetworkReply::errorString() const
{
    const QFtpNetworkReplyPrivate *d = d_func();
    return d->errorString;
}

bool QFtpNetworkReply::isFinished() const
{
    const QFtpNetworkReplyPrivate *d = d_func();
    return d->finished;
}

bool QFtpNetworkReply::isRunning() const
{
    const QFtpNetworkReplyPrivate *d = d_func();
    return d->running.loadRelaxed() != 0;
}

QByteArray QFtpNetworkReply::readAll()
{
    QFtpNetworkReplyPrivate *d = d_func();
    QByteArray data = d->buffer;
    d->buffer.clear();
    return data;
}

qint64 QFtpNetworkReply::bytesAvailable() const
{
    const QFtpNetworkReplyPrivate *d = d_func();
    return d->buffer.size();
}

void QFtpNetworkReply::abort()
{
    QFtpNetworkReplyPrivate *d = d_func();
    if (d->curl) {
        // Set running to false to signal the progress callback to abort
        d->running.storeRelaxed(0);
    }
}
