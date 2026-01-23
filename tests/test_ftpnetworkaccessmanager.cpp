#include <QtTest/QtTest>
#include <QFtpClient/qftpnetworkaccessmanager.h>
#include <QFtpClient/qftpnetworkreply.h>
#include <QBuffer>

class TestFtpNetworkAccessManager : public QObject
{
    Q_OBJECT

private slots:
    void testConstruction()
    {
        QFtpNetworkAccessManager manager;
        QVERIFY(manager.userName().isEmpty());
        QVERIFY(manager.password().isEmpty());
        QVERIFY(!manager.sslCertificateVerification());
    }

    void testCredentials()
    {
        QFtpNetworkAccessManager manager;
        
        QString testUser = "testuser";
        QString testPass = "testpass";
        
        manager.setUserName(testUser);
        manager.setPassword(testPass);
        
        QCOMPARE(manager.userName(), testUser);
        QCOMPARE(manager.password(), testPass);
    }

    void testSslVerification()
    {
        QFtpNetworkAccessManager manager;
        
        QVERIFY(!manager.sslCertificateVerification());
        
        manager.setSslCertificateVerification(true);
        QVERIFY(manager.sslCertificateVerification());
        
        manager.setSslCertificateVerification(false);
        QVERIFY(!manager.sslCertificateVerification());
    }

    void testGetOperation()
    {
        QFtpNetworkAccessManager manager;
        QNetworkRequest request(QUrl("ftp://test.example.com/file.txt"));
        
        QFtpNetworkReply *reply = manager.get(request);
        
        QVERIFY(reply != nullptr);
        QCOMPARE(reply->operation(), QFtpNetworkReply::RetrieveOperation);
        QCOMPARE(reply->url(), request.url());
        
        reply->abort();
        reply->deleteLater();
    }

    void testPutOperation()
    {
        QFtpNetworkAccessManager manager;
        QNetworkRequest request(QUrl("ftp://test.example.com/upload.txt"));
        
        QByteArray data("Test data");
        QBuffer buffer(&data);
        buffer.open(QIODevice::ReadOnly);
        
        QFtpNetworkReply *reply = manager.put(request, &buffer);
        
        QVERIFY(reply != nullptr);
        QCOMPARE(reply->operation(), QFtpNetworkReply::StoreOperation);
        QCOMPARE(reply->url(), request.url());
        
        reply->abort();
        QTest::qWait(100);
        reply->deleteLater();
    }

    void testDeleteOperation()
    {
        QFtpNetworkAccessManager manager;
        QNetworkRequest request(QUrl("ftp://test.example.com/delete.txt"));
        
        QFtpNetworkReply *reply = manager.deleteResource(request);
        
        QVERIFY(reply != nullptr);
        QCOMPARE(reply->operation(), QFtpNetworkReply::RemoveOperation);
        QCOMPARE(reply->url(), request.url());
        
        reply->abort();
        reply->deleteLater();
    }
};

QTEST_GUILESS_MAIN(TestFtpNetworkAccessManager)
#include "test_ftpnetworkaccessmanager.moc"
