#ifndef QFTPNETWORKREPLY_P_H
#define QFTPNETWORKREPLY_P_H

#include "QFtpClient/qftpnetworkreply.h"
#include <QUrl>
#include <QByteArray>
#include <QThread>
#include <QIODevice>
#include <QAtomicInt>
#include <curl/curl.h>

class QFtpNetworkReplyPrivate
{
public:
    QUrl url;
    QFtpNetworkReply::Operation operation;
    QFtpNetworkReply::NetworkError errorCode;
    QString errorString;
    bool finished;
    QAtomicInt running;
    QByteArray buffer;
    QIODevice *sourceData;
    CURL *curl;
    QString userName;
    QString password;
    QThread *workerThread;
    bool verifySslCertificate;
    
    QFtpNetworkReplyPrivate()
        : operation(QFtpNetworkReply::GetOperation),
          errorCode(QFtpNetworkReply::NoError),
          finished(false),
          running(0),
          sourceData(nullptr),
          curl(nullptr),
          workerThread(nullptr),
          verifySslCertificate(false)
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
            // Check if operation was aborted
            if (reply->d_ptr && !reply->d_ptr->running.loadRelaxed()) {
                return 1; // Return non-zero to abort the transfer
            }
            
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

#endif // QFTPNETWORKREPLY_P_H
