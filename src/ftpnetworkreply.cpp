#include "FtpClient/ftpnetworkreply.h"
#include "private/ftpnetworkreply_p.h"

FtpNetworkReply::FtpNetworkReply(QObject *parent)
    : QObject(parent), d_ptr(new FtpNetworkReplyPrivate())
{
}

FtpNetworkReply::~FtpNetworkReply()
{
    if (d_ptr) {
        delete d_ptr;
    }
}

QUrl FtpNetworkReply::url() const
{
    Q_D(const FtpNetworkReply);
    return d->url;
}

FtpNetworkReply::Operation FtpNetworkReply::operation() const
{
    Q_D(const FtpNetworkReply);
    return d->operation;
}

FtpNetworkReply::NetworkError FtpNetworkReply::error() const
{
    Q_D(const FtpNetworkReply);
    return d->errorCode;
}

QString FtpNetworkReply::errorString() const
{
    Q_D(const FtpNetworkReply);
    return d->errorString;
}

bool FtpNetworkReply::isFinished() const
{
    Q_D(const FtpNetworkReply);
    return d->finished;
}

bool FtpNetworkReply::isRunning() const
{
    Q_D(const FtpNetworkReply);
    return d->running.loadRelaxed() != 0;
}

QByteArray FtpNetworkReply::readAll()
{
    Q_D(FtpNetworkReply);
    QByteArray data = d->buffer;
    d->buffer.clear();
    return data;
}

qint64 FtpNetworkReply::bytesAvailable() const
{
    Q_D(const FtpNetworkReply);
    return d->buffer.size();
}

void FtpNetworkReply::abort()
{
    Q_D(FtpNetworkReply);
    if (d->curl) {
        // Set running to false to signal the progress callback to abort
        d->running.storeRelaxed(0);
    }
}
