
message(Build custom FTP Client)
# Source files
SOURCES += \
    src/ftpnetworkaccessmanager.cpp \
    src/ftpnetworkreply.cpp

# Header files
HEADERS += \
    include/QFtpClient/ftpnetworkaccessmanager.h \
    include/QFtpClient/ftpnetworkreply.h \
    include/QFtpClient/FtpClient

# Include directories
INCLUDEPATH += $$PWD/include

INCLUDEPATH += $$PWD/src

# libcurl configuration
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libcurl
}
