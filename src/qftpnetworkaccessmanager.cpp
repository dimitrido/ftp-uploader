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
    : QObject(parent), m_impl(new QFtpNetworkAccessManagerPrivate())
{
}

QFtpNetworkAccessManager::~QFtpNetworkAccessManager()
{
    // unique_ptr automatically cleans up
}

QFtpNetworkReply* QFtpNetworkAccessManager::get(const QNetworkRequest &request)
{
    QFtpNetworkReply *reply = new QFtpNetworkReply(this);
    reply->m_impl->url = request.url();
    reply->m_impl->operation = QFtpNetworkReply::RetrieveOperation;
    reply->m_impl->userName = m_impl->userName;
    reply->m_impl->password = m_impl->password;
    reply->m_impl->verifySslCertificate = m_impl->verifySslCertificate;
    
    // Perform operation in a separate thread
    QThread *thread = QThread::create([reply]() {
        QFtpNetworkReplyPrivate *rd = reply->m_impl.get();
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
                rd->errorCode = QFtpNetworkReply::FtpTransferFailed;
                rd->errorString = QString::fromUtf8(curl_easy_strerror(res));
            }
            
            QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        }
    });
    
    reply->m_impl->workerThread = thread;
    thread->start();
    
    return reply;
}

QFtpNetworkReply* QFtpNetworkAccessManager::put(const QNetworkRequest &request, QIODevice *data)
{
    QFtpNetworkReply *reply = new QFtpNetworkReply(this);
    reply->m_impl->url = request.url();
    reply->m_impl->operation = QFtpNetworkReply::StoreOperation;
    reply->m_impl->sourceData = data;
    reply->m_impl->userName = m_impl->userName;
    reply->m_impl->password = m_impl->password;
    reply->m_impl->verifySslCertificate = m_impl->verifySslCertificate;
    
    // Perform operation in a separate thread
    QThread *thread = QThread::create([reply]() {
        QFtpNetworkReplyPrivate *rd = reply->m_impl.get();
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
                rd->errorCode = QFtpNetworkReply::FtpTransferFailed;
                rd->errorString = QString::fromUtf8(curl_easy_strerror(res));
            }
            
            QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        }
    });
    
    reply->m_impl->workerThread = thread;
    thread->start();
    
    return reply;
}

QFtpNetworkReply* QFtpNetworkAccessManager::deleteResource(const QNetworkRequest &request)
{
    QFtpNetworkReply *reply = new QFtpNetworkReply(this);
    reply->m_impl->url = request.url();
    reply->m_impl->operation = QFtpNetworkReply::RemoveOperation;
    reply->m_impl->userName = m_impl->userName;
    reply->m_impl->password = m_impl->password;
    reply->m_impl->verifySslCertificate = m_impl->verifySslCertificate;
    
    // Perform operation in a separate thread
    QThread *thread = QThread::create([reply]() {
        QFtpNetworkReplyPrivate *rd = reply->m_impl.get();
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
                rd->errorCode = QFtpNetworkReply::FtpTransferFailed;
                rd->errorString = QString::fromUtf8(curl_easy_strerror(res));
            }
            
            QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        }
    });
    
    reply->m_impl->workerThread = thread;
    thread->start();
    
    return reply;
}

void QFtpNetworkAccessManager::setUserName(const QString &userName)
{
    m_impl->userName = userName;
}

void QFtpNetworkAccessManager::setPassword(const QString &password)
{
    m_impl->password = password;
}

QString QFtpNetworkAccessManager::userName() const
{
    return m_impl->userName;
}

QString QFtpNetworkAccessManager::password() const
{
    return m_impl->password;
}

void QFtpNetworkAccessManager::setSslCertificateVerification(bool verify)
{
    m_impl->verifySslCertificate = verify;
}

bool QFtpNetworkAccessManager::sslCertificateVerification() const
{
    return m_impl->verifySslCertificate;
}
