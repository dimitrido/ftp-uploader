#include <QtTest/QtTest>
#include <QFtpClient/qftpnetworkreply.h>

class TestFtpError : public QObject
{
    Q_OBJECT

private slots:
    void testErrorCodeValues()
    {
        // Test that error codes have expected values
        QCOMPARE(static_cast<int>(QFtpNetworkReply::FtpNoError), 0);
        QCOMPARE(static_cast<int>(QFtpNetworkReply::FtpUnknownError), 99);
        QCOMPARE(static_cast<int>(QFtpNetworkReply::FtpProtocolError), 301);
        QCOMPARE(static_cast<int>(QFtpNetworkReply::FtpAuthenticationFailed), 401);
        QCOMPARE(static_cast<int>(QFtpNetworkReply::FtpAccessDenied), 403);
        QCOMPARE(static_cast<int>(QFtpNetworkReply::FtpResourceNotFound), 404);
        QCOMPARE(static_cast<int>(QFtpNetworkReply::FtpOperationNotAllowed), 405);
        QCOMPARE(static_cast<int>(QFtpNetworkReply::FtpTransferFailed), 499);
    }

    void testOperationValues()
    {
        // Test operation enum values are distinct
        QVERIFY(QFtpNetworkReply::RetrieveOperation != QFtpNetworkReply::StoreOperation);
        QVERIFY(QFtpNetworkReply::StoreOperation != QFtpNetworkReply::RemoveOperation);
        QVERIFY(QFtpNetworkReply::RetrieveOperation != QFtpNetworkReply::RemoveOperation);
    }
};

QTEST_GUILESS_MAIN(TestFtpError)
#include "test_ftperror.moc"
