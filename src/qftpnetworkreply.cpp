#include "QFtpClient/qftpnetworkreply.h"
#include "private/qftpnetworkreply_p.h"

QFtpNetworkReply::QFtpNetworkReply(QObject *parent)
    : QObject(parent), m_impl(new QFtpNetworkReplyPrivate())
{
}

QFtpNetworkReply::~QFtpNetworkReply()
{
    // unique_ptr automatically cleans up
}

QUrl QFtpNetworkReply::url() const
{
    return m_impl->url;
}

QFtpNetworkReply::FtpOperation QFtpNetworkReply::operation() const
{
    return m_impl->operation;
}

QFtpNetworkReply::FtpError QFtpNetworkReply::error() const
{
    return m_impl->errorCode;
}

QString QFtpNetworkReply::errorString() const
{
    return m_impl->errorString;
}

bool QFtpNetworkReply::isFinished() const
{
    return m_impl->finished;
}

bool QFtpNetworkReply::isRunning() const
{
    return m_impl->running.loadRelaxed() != 0;
}

QByteArray QFtpNetworkReply::readAll()
{
    QByteArray data = m_impl->buffer;
    m_impl->buffer.clear();
    return data;
}

qint64 QFtpNetworkReply::bytesAvailable() const
{
    return m_impl->buffer.size();
}

void QFtpNetworkReply::abort()
{
    if (m_impl->curl) {
        // Set running to false to signal the progress callback to abort
        m_impl->running.storeRelaxed(0);
    }
}
