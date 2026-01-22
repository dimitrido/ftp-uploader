TEMPLATE = lib
TARGET = QFtpClient
VERSION = 1.0.0

QT += core network
QT -= gui

CONFIG += c++11

# Define the library type
CONFIG += shared

# Source files
SOURCES += \
    src/qftpnetworkaccessmanager.cpp \
    src/qftpnetworkreply.cpp

# Header files
HEADERS += \
    include/QFtpClient/qftpnetworkaccessmanager.h \
    include/QFtpClient/qftpnetworkreply.h \
    include/QFtpClient/QFtpClient

# Include directories
INCLUDEPATH += include

# libcurl configuration
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libcurl
}

win32 {
    # Windows: Adjust these paths according to your libcurl installation
    INCLUDEPATH += "C:/curl/include"
    LIBS += -L"C:/curl/lib" -lcurl
}

# Installation paths
unix {
    target.path = /usr/lib
    headers.path = /usr/include/QFtpClient
    headers.files = $$HEADERS
    INSTALLS += target headers
}

win32 {
    target.path = C:/Qt/QFtpClient/lib
    headers.path = C:/Qt/QFtpClient/include/QFtpClient
    headers.files = $$HEADERS
    INSTALLS += target headers
}
