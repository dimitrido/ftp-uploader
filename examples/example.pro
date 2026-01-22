TEMPLATE = app
TARGET = QFtpClientExample
QT += core network
QT -= gui

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

# Add parent directory to include path to find QFtpClient
INCLUDEPATH += ../include

# Link against QFtpClient library
LIBS += -L../lib -lQFtpClient

# Source files
SOURCES += example.cpp
