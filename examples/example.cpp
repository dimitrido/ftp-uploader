#include <QCoreApplication>
#include <QNetworkRequest>
#include <QFile>
#include <QDebug>
#include <QFtpClient/QFtpClient>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // Create FTP manager
    QFtpNetworkAccessManager manager;
    manager.setUserName("anonymous");
    manager.setPassword("user@example.com");
    
    qDebug() << "QFtpClient Example";
    qDebug() << "==================";
    qDebug() << "";
    qDebug() << "This example demonstrates how to use QFtpClient library.";
    qDebug() << "";
    qDebug() << "Example 1: Download a file from FTP server";
    
    // Example 1: Download a file
    QNetworkRequest request(QUrl("ftp://ftp.example.com/path/to/file.txt"));
    QFtpNetworkReply *reply = manager.get(request);
    
    QObject::connect(reply, &QFtpNetworkReply::downloadProgress, 
                     [](qint64 bytesReceived, qint64 bytesTotal) {
        qDebug() << "Download progress:" << bytesReceived << "/" << bytesTotal;
    });
    
    QObject::connect(reply, &QFtpNetworkReply::finished, [reply, &app]() {
        if (reply->error() == QFtpNetworkReply::FtpNoError) {
            QByteArray data = reply->readAll();
            qDebug() << "Download completed successfully!";
            qDebug() << "Downloaded" << data.size() << "bytes";
        } else {
            qDebug() << "Download failed:" << reply->errorString();
        }
        reply->deleteLater();
        app.quit();
    });
    
    qDebug() << "";
    qDebug() << "Example 2: Upload a file (commented out)";
    qDebug() << "To upload a file, use:";
    qDebug() << "  QFile *file = new QFile(\"local_file.txt\");";
    qDebug() << "  file->open(QIODevice::ReadOnly);";
    qDebug() << "  QNetworkRequest uploadRequest(QUrl(\"ftp://ftp.example.com/remote_file.txt\"));";
    qDebug() << "  QFtpNetworkReply *uploadReply = manager.put(uploadRequest, file);";
    qDebug() << "";
    qDebug() << "Supported protocols:";
    qDebug() << "  - ftp://  (standard FTP)";
    qDebug() << "  - ftps:// (FTP over SSL/TLS)";
    qDebug() << "  - sftp:// (SSH File Transfer Protocol)";
    qDebug() << "";
    
    return app.exec();
}
