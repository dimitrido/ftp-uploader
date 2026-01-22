#include "QFtpClient/qftpnetworkreply.h"
#include <QUrl>
#include <QByteArray>
#include <QThread>
#include <QIODevice>
#include <curl/curl.h>

class QFtpNetworkReplyPrivate
{
public:
    QUrl url;
    QFtpNetworkReply::Operation operation;
    QFtpNetworkReply::NetworkError errorCode;
    QString errorString;
    bool finished;
    bool running;
    QByteArray buffer;
    QIODevice *sourceData;
    CURL *curl;
    QString userName;
    QString password;
    QThread *workerThread;
    
    QFtpNetworkReplyPrivate()
        : operation(QFtpNetworkReply::GetOperation),
          errorCode(QFtpNetworkReply::NoError),
          finished(false),
          running(false),
          sourceData(nullptr),
          curl(nullptr),
          workerThread(nullptr)
    {
    }
    
    ~QFtpNetworkReplyPrivate()
    {
        if (curl) {
            curl_easy_cleanup(curl);
        }
        if (workerThread) {
            workerThread->quit();
            workerThread->wait();
            delete workerThread;
        }
    }
    
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t realsize = size * nmemb;
        QByteArray *buffer = static_cast<QByteArray*>(userp);
        buffer->append(static_cast<char*>(contents), realsize);
        return realsize;
    }
    
    static size_t readCallback(void *ptr, size_t size, size_t nmemb, void *userp)
    {
        QIODevice *device = static_cast<QIODevice*>(userp);
        if (!device || !device->isOpen()) {
            return 0;
        }
        
        size_t maxBytes = size * nmemb;
        qint64 bytesRead = device->read(static_cast<char*>(ptr), maxBytes);
        return bytesRead > 0 ? bytesRead : 0;
    }
    
    static int progressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                               curl_off_t ultotal, curl_off_t ulnow)
    {
        QFtpNetworkReply *reply = static_cast<QFtpNetworkReply*>(clientp);
        if (reply) {
            if (dltotal > 0) {
                QMetaObject::invokeMethod(reply, "downloadProgress", Qt::QueuedConnection,
                    Q_ARG(qint64, dlnow), Q_ARG(qint64, dltotal));
            }
            if (ultotal > 0) {
                QMetaObject::invokeMethod(reply, "uploadProgress", Qt::QueuedConnection,
                    Q_ARG(qint64, ulnow), Q_ARG(qint64, ultotal));
            }
        }
        return 0;
    }
};

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
