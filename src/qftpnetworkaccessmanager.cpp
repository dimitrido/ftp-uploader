#include "QFtpClient/qftpnetworkaccessmanager.h"
#include "QFtpClient/qftpnetworkreply.h"
#include "private/qftpnetworkreply_p.h"
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <curl/curl.h>

class QFtpNetworkAccessManagerPrivate
{
public:
    QString userName;
    QString password;
    bool verifySslCertificate;
    
    QFtpNetworkAccessManagerPrivate()
        : verifySslCertificate(false)
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }
    
    ~QFtpNetworkAccessManagerPrivate()
    {
        curl_global_cleanup();
    }
};

QFtpNetworkAccessManager::QFtpNetworkAccessManager(QObject *parent)
    : QObject(parent), d_ptr(new QFtpNetworkAccessManagerPrivate())
{
}

QFtpNetworkAccessManager::~QFtpNetworkAccessManager()
{
    delete d_ptr;
}

QFtpNetworkReply* QFtpNetworkAccessManager::get(const QNetworkRequest &request)
{
    Q_D(QFtpNetworkAccessManager);
    
    QFtpNetworkReply *reply = new QFtpNetworkReply(this);
    reply->d_ptr->url = request.url();
    reply->d_ptr->operation = QFtpNetworkReply::GetOperation;
    reply->d_ptr->userName = d->userName;
    reply->d_ptr->password = d->password;
    reply->d_ptr->verifySslCertificate = d->verifySslCertificate;
    
    // Perform operation in a separate thread
    QThread *thread = QThread::create([reply]() {
        QFtpNetworkReplyPrivate *rd = reply->d_ptr;
        rd->curl = curl_easy_init();
        
        if (rd->curl) {
            QString urlStr = rd->url.toString();
            curl_easy_setopt(rd->curl, CURLOPT_URL, urlStr.toUtf8().constData());
            curl_easy_setopt(rd->curl, CURLOPT_WRITEFUNCTION, QFtpNetworkReplyPrivate::writeCallback);
            curl_easy_setopt(rd->curl, CURLOPT_WRITEDATA, &rd->buffer);
            curl_easy_setopt(rd->curl, CURLOPT_XFERINFOFUNCTION, QFtpNetworkReplyPrivate::progressCallback);
            curl_easy_setopt(rd->curl, CURLOPT_XFERINFODATA, reply);
            curl_easy_setopt(rd->curl, CURLOPT_NOPROGRESS, 0L);
            
            // Set authentication if provided
            if (!rd->userName.isEmpty()) {
                curl_easy_setopt(rd->curl, CURLOPT_USERNAME, rd->userName.toUtf8().constData());
                curl_easy_setopt(rd->curl, CURLOPT_PASSWORD, rd->password.toUtf8().constData());
            }
            
            // Handle FTPS (use SSL)
            if (rd->url.scheme() == "ftps") {
                curl_easy_setopt(rd->curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
                if (rd->verifySslCertificate) {
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYPEER, 1L);
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYHOST, 2L);
                } else {
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYPEER, 0L);
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYHOST, 0L);
                }
            }
            // SFTP is handled automatically by libcurl when scheme is "sftp"
            
            rd->running.storeRelaxed(1);
            CURLcode res = curl_easy_perform(rd->curl);
            rd->running.storeRelaxed(0);
            rd->finished = true;
            
            if (res != CURLE_OK) {
                rd->errorCode = QFtpNetworkReply::ProtocolFailure;
                rd->errorString = QString::fromUtf8(curl_easy_strerror(res));
            }
            
            QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        }
    });
    
    reply->d_ptr->workerThread = thread;
    thread->start();
    
    return reply;
}

QFtpNetworkReply* QFtpNetworkAccessManager::put(const QNetworkRequest &request, QIODevice *data)
{
    Q_D(QFtpNetworkAccessManager);
    
    QFtpNetworkReply *reply = new QFtpNetworkReply(this);
    reply->d_ptr->url = request.url();
    reply->d_ptr->operation = QFtpNetworkReply::PutOperation;
    reply->d_ptr->sourceData = data;
    reply->d_ptr->userName = d->userName;
    reply->d_ptr->password = d->password;
    reply->d_ptr->verifySslCertificate = d->verifySslCertificate;
    
    // Perform operation in a separate thread
    QThread *thread = QThread::create([reply]() {
        QFtpNetworkReplyPrivate *rd = reply->d_ptr;
        rd->curl = curl_easy_init();
        
        if (rd->curl) {
            QString urlStr = rd->url.toString();
            curl_easy_setopt(rd->curl, CURLOPT_URL, urlStr.toUtf8().constData());
            curl_easy_setopt(rd->curl, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(rd->curl, CURLOPT_READFUNCTION, QFtpNetworkReplyPrivate::readCallback);
            curl_easy_setopt(rd->curl, CURLOPT_READDATA, rd->sourceData);
            curl_easy_setopt(rd->curl, CURLOPT_XFERINFOFUNCTION, QFtpNetworkReplyPrivate::progressCallback);
            curl_easy_setopt(rd->curl, CURLOPT_XFERINFODATA, reply);
            curl_easy_setopt(rd->curl, CURLOPT_NOPROGRESS, 0L);
            
            if (rd->sourceData && rd->sourceData->isOpen()) {
                curl_easy_setopt(rd->curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)rd->sourceData->size());
            }
            
            // Set authentication if provided
            if (!rd->userName.isEmpty()) {
                curl_easy_setopt(rd->curl, CURLOPT_USERNAME, rd->userName.toUtf8().constData());
                curl_easy_setopt(rd->curl, CURLOPT_PASSWORD, rd->password.toUtf8().constData());
            }
            
            // Handle FTPS (use SSL)
            if (rd->url.scheme() == "ftps") {
                curl_easy_setopt(rd->curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
                if (rd->verifySslCertificate) {
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYPEER, 1L);
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYHOST, 2L);
                } else {
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYPEER, 0L);
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYHOST, 0L);
                }
            }
            
            rd->running.storeRelaxed(1);
            CURLcode res = curl_easy_perform(rd->curl);
            rd->running.storeRelaxed(0);
            rd->finished = true;
            
            if (res != CURLE_OK) {
                rd->errorCode = QFtpNetworkReply::ProtocolFailure;
                rd->errorString = QString::fromUtf8(curl_easy_strerror(res));
            }
            
            QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        }
    });
    
    reply->d_ptr->workerThread = thread;
    thread->start();
    
    return reply;
}

QFtpNetworkReply* QFtpNetworkAccessManager::deleteResource(const QNetworkRequest &request)
{
    Q_D(QFtpNetworkAccessManager);
    
    QFtpNetworkReply *reply = new QFtpNetworkReply(this);
    reply->d_ptr->url = request.url();
    reply->d_ptr->operation = QFtpNetworkReply::DeleteOperation;
    reply->d_ptr->userName = d->userName;
    reply->d_ptr->password = d->password;
    reply->d_ptr->verifySslCertificate = d->verifySslCertificate;
    
    // Perform operation in a separate thread
    QThread *thread = QThread::create([reply]() {
        QFtpNetworkReplyPrivate *rd = reply->d_ptr;
        rd->curl = curl_easy_init();
        
        if (rd->curl) {
            QString urlStr = rd->url.toString();
            
            // For FTP DELETE, we need to use QUOTE command
            struct curl_slist *headerlist = nullptr;
            QString deleteCmd = "DELE " + rd->url.path();
            headerlist = curl_slist_append(headerlist, deleteCmd.toUtf8().constData());
            
            // Get the base URL (without the file)
            QUrl baseUrl = rd->url;
            QString path = baseUrl.path();
            int lastSlash = path.lastIndexOf('/');
            if (lastSlash >= 0) {
                baseUrl.setPath(path.left(lastSlash + 1));
            }
            
            curl_easy_setopt(rd->curl, CURLOPT_URL, baseUrl.toString().toUtf8().constData());
            curl_easy_setopt(rd->curl, CURLOPT_QUOTE, headerlist);
            curl_easy_setopt(rd->curl, CURLOPT_NOBODY, 1L);
            
            // Set authentication if provided
            if (!rd->userName.isEmpty()) {
                curl_easy_setopt(rd->curl, CURLOPT_USERNAME, rd->userName.toUtf8().constData());
                curl_easy_setopt(rd->curl, CURLOPT_PASSWORD, rd->password.toUtf8().constData());
            }
            
            // Handle FTPS (use SSL)
            if (rd->url.scheme() == "ftps") {
                curl_easy_setopt(rd->curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
                if (rd->verifySslCertificate) {
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYPEER, 1L);
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYHOST, 2L);
                } else {
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYPEER, 0L);
                    curl_easy_setopt(rd->curl, CURLOPT_SSL_VERIFYHOST, 0L);
                }
            }
            
            rd->running.storeRelaxed(1);
            CURLcode res = curl_easy_perform(rd->curl);
            rd->running.storeRelaxed(0);
            rd->finished = true;
            
            curl_slist_free_all(headerlist);
            
            if (res != CURLE_OK) {
                rd->errorCode = QFtpNetworkReply::ProtocolFailure;
                rd->errorString = QString::fromUtf8(curl_easy_strerror(res));
            }
            
            QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        }
    });
    
    reply->d_ptr->workerThread = thread;
    thread->start();
    
    return reply;
}

void QFtpNetworkAccessManager::setUserName(const QString &userName)
{
    Q_D(QFtpNetworkAccessManager);
    d->userName = userName;
}

void QFtpNetworkAccessManager::setPassword(const QString &password)
{
    Q_D(QFtpNetworkAccessManager);
    d->password = password;
}

QString QFtpNetworkAccessManager::userName() const
{
    Q_D(const QFtpNetworkAccessManager);
    return d->userName;
}

QString QFtpNetworkAccessManager::password() const
{
    Q_D(const QFtpNetworkAccessManager);
    return d->password;
}

void QFtpNetworkAccessManager::setSslCertificateVerification(bool verify)
{
    Q_D(QFtpNetworkAccessManager);
    d->verifySslCertificate = verify;
}

bool QFtpNetworkAccessManager::sslCertificateVerification() const
{
    Q_D(const QFtpNetworkAccessManager);
    return d->verifySslCertificate;
}
