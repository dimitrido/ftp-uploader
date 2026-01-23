#include <QtTest/QtTest>
#include <QFtpClient/qftpnetworkreply.h>
#include <QFtpClient/qftpnetworkaccessmanager.h>
#include <QSignalSpy>

class TestFtpNetworkReply : public QObject
{
    Q_OBJECT

private slots:
    void testInitialState()
    {
        QFtpNetworkAccessManager manager;
        QNetworkRequest request(QUrl("ftp://test.example.com/file.txt"));
        QFtpNetworkReply *reply = manager.get(request);
        
        // Check initial state
        QVERIFY(reply != nullptr);
        QCOMPARE(reply->url(), request.url());
        QCOMPARE(reply->operation(), QFtpNetworkReply::RetrieveOperation);
        QCOMPARE(reply->error(), QFtpNetworkReply::FtpNoError);
        QVERIFY(!reply->isFinished());
        
        // Clean up
        reply->abort();
        QTest::qWait(100);
        reply->deleteLater();
    }

    void testSignals()
    {
        QFtpNetworkAccessManager manager;
        QNetworkRequest request(QUrl("ftp://invalid.test.example.com/file.txt"));
        QFtpNetworkReply *reply = manager.get(request);
        
        // Set up signal spies
        QSignalSpy finishedSpy(reply, &QFtpNetworkReply::finished);
        QSignalSpy downloadProgressSpy(reply, &QFtpNetworkReply::downloadProgress);
        
        // Wait for operation to complete (will fail due to invalid URL)
        QVERIFY(finishedSpy.wait(5000));
        QCOMPARE(finishedSpy.count(), 1);
        
        // Check error state
        QVERIFY(reply->error() != QFtpNetworkReply::FtpNoError);
        QVERIFY(!reply->errorString().isEmpty());
        QVERIFY(reply->isFinished());
        
        reply->deleteLater();
    }

    void testAbort()
    {
        QFtpNetworkAccessManager manager;
        QNetworkRequest request(QUrl("ftp://test.example.com/largefile.bin"));
        QFtpNetworkReply *reply = manager.get(request);
        
        // Abort immediately
        QVERIFY(reply->isRunning() || !reply->isFinished());
        reply->abort();
        
        // Wait a bit
        QTest::qWait(200);
        
        // Should be finished after abort
        QVERIFY(!reply->isRunning() || reply->isFinished());
        
        reply->deleteLater();
    }
};

QTEST_GUILESS_MAIN(TestFtpNetworkReply)
#include "test_ftpnetworkreply.moc"
