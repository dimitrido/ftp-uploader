# QFtpClient

A modern C++ FTP client library with Qt API, based on libcurl.

## Features

- **Qt-style API**: API design similar to QNetworkAccessManager for familiar usage
- **Multiple Protocol Support**: 
  - FTP (File Transfer Protocol)
  - FTPS (FTP over SSL/TLS)
  - SFTP (SSH File Transfer Protocol)
- **Dual Build System Support**: Compatible with both CMake and qmake
- **Qt Version Compatibility**: Works with both Qt 5 and Qt 6
- **Asynchronous Operations**: Non-blocking file transfers with progress callbacks
- **libcurl Backend**: Robust and well-tested implementation

## Requirements

- Qt 5.x or Qt 6.x (Core and Network modules)
- libcurl (with SSL and SSH support for FTPS/SFTP)
- C++11 compatible compiler
- CMake 3.10+ (if using CMake) or qmake (if using qmake)

## Building with CMake

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

To disable building examples:
```bash
cmake -DBUILD_EXAMPLES=OFF ..
```

## Building with qmake

```bash
qmake QFtpClient.pro
make
sudo make install
```

## Usage

### Basic Example

```cpp
#include <QCoreApplication>
#include <QFtpClient/QFtpClient>
#include <QNetworkRequest>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // Create FTP manager
    QFtpNetworkAccessManager manager;
    manager.setUserName("username");
    manager.setPassword("password");
    
    // Download a file
    QNetworkRequest request(QUrl("ftp://ftp.example.com/file.txt"));
    QFtpNetworkReply *reply = manager.get(request);
    
    QObject::connect(reply, &QFtpNetworkReply::finished, [reply]() {
        if (reply->error() == QFtpNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            qDebug() << "Downloaded" << data.size() << "bytes";
        } else {
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
    
    return app.exec();
}
```

### Upload Example

```cpp
// Upload a file
QFile *file = new QFile("local_file.txt");
if (file->open(QIODevice::ReadOnly)) {
    QNetworkRequest uploadRequest(QUrl("ftp://ftp.example.com/remote_file.txt"));
    QFtpNetworkReply *uploadReply = manager.put(uploadRequest, file);
    
    QObject::connect(uploadReply, &QFtpNetworkReply::uploadProgress, 
                     [](qint64 bytesSent, qint64 bytesTotal) {
        qDebug() << "Upload progress:" << bytesSent << "/" << bytesTotal;
    });
    
    QObject::connect(uploadReply, &QFtpNetworkReply::finished, [uploadReply, file]() {
        if (uploadReply->error() == QFtpNetworkReply::NoError) {
            qDebug() << "Upload completed!";
        } else {
            qDebug() << "Upload failed:" << uploadReply->errorString();
        }
        file->close();
        file->deleteLater();
        uploadReply->deleteLater();
    });
}
```

### FTPS Example

```cpp
// Use FTPS (FTP over SSL/TLS)
QNetworkRequest secureRequest(QUrl("ftps://secure.example.com/file.txt"));
QFtpNetworkReply *secureReply = manager.get(secureRequest);
```

### SFTP Example

```cpp
// Use SFTP (SSH File Transfer Protocol)
QNetworkRequest sftpRequest(QUrl("sftp://ssh.example.com/file.txt"));
QFtpNetworkReply *sftpReply = manager.get(sftpRequest);
```

### Delete File Example

```cpp
// Delete a file
QNetworkRequest deleteRequest(QUrl("ftp://ftp.example.com/file_to_delete.txt"));
QFtpNetworkReply *deleteReply = manager.deleteResource(deleteRequest);

QObject::connect(deleteReply, &QFtpNetworkReply::finished, [deleteReply]() {
    if (deleteReply->error() == QFtpNetworkReply::NoError) {
        qDebug() << "File deleted successfully!";
    } else {
        qDebug() << "Delete failed:" << deleteReply->errorString();
    }
    deleteReply->deleteLater();
});
```

## API Reference

### QFtpNetworkAccessManager

Main class for managing FTP operations.

**Methods:**
- `QFtpNetworkReply* get(const QNetworkRequest &request)` - Download a file
- `QFtpNetworkReply* put(const QNetworkRequest &request, QIODevice *data)` - Upload a file
- `QFtpNetworkReply* deleteResource(const QNetworkRequest &request)` - Delete a file
- `void setUserName(const QString &userName)` - Set FTP username
- `void setPassword(const QString &password)` - Set FTP password
- `QString userName() const` - Get current username
- `QString password() const` - Get current password

**Signals:**
- `void finished(QFtpNetworkReply *reply)` - Emitted when an operation finishes

### QFtpNetworkReply

Represents an ongoing or completed FTP operation.

**Methods:**
- `QUrl url() const` - Get the URL of the request
- `Operation operation() const` - Get the operation type (Get, Put, Delete)
- `NetworkError error() const` - Get error code
- `QString errorString() const` - Get error description
- `bool isFinished() const` - Check if operation is finished
- `bool isRunning() const` - Check if operation is running
- `QByteArray readAll()` - Read all downloaded data
- `qint64 bytesAvailable() const` - Get number of bytes available to read
- `void abort()` - Abort the operation

**Signals:**
- `void finished()` - Emitted when operation completes
- `void error(QFtpNetworkReply::NetworkError code)` - Emitted on error
- `void downloadProgress(qint64 bytesReceived, qint64 bytesTotal)` - Download progress
- `void uploadProgress(qint64 bytesSent, qint64 bytesTotal)` - Upload progress

## Protocol Support

### FTP (ftp://)
Standard File Transfer Protocol. Use this for basic, unencrypted file transfers.

### FTPS (ftps://)
FTP over SSL/TLS. Provides encrypted data transfer. The library automatically configures SSL when using the `ftps://` scheme.

### SFTP (sftp://)
SSH File Transfer Protocol. Requires libcurl to be compiled with SSH support. Uses SSH for secure, encrypted file transfers.

## Dependencies

### libcurl

QFtpClient requires libcurl with the following features:
- SSL/TLS support (for FTPS)
- SSH support (for SFTP)

**Ubuntu/Debian:**
```bash
sudo apt-get install libcurl4-openssl-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install libcurl-devel
```

**macOS (Homebrew):**
```bash
brew install curl
```

**Windows:**
Download pre-built libcurl from https://curl.se/windows/

## License

This project is provided as-is for use with any license compatible with Qt and libcurl.

## Contributing

Contributions are welcome! Please ensure your code follows the existing style and includes appropriate tests.

## Troubleshooting

### SSL Certificate Errors
By default, SSL certificate verification is disabled for FTPS connections. To enable verification:
```cpp
// You'll need to modify the library code to enable certificate verification
// Look for CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options
```

### SFTP Authentication
SFTP typically uses SSH key authentication. Make sure:
1. Your SSH keys are properly configured
2. libcurl is compiled with SSH support
3. The server accepts your authentication method

### Build Errors
- Ensure Qt development packages are installed
- Verify libcurl is installed with SSL and SSH support
- Check that pkg-config can find libcurl (Linux)

## Examples

See the `examples/` directory for complete working examples.
