cmake_minimum_required(VERSION 3.10)
project(QFtpClientExample)

# Find Qt packages
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)

# Enable automatic MOC
set(CMAKE_AUTOMOC ON)

# Create the example executable
add_executable(${PROJECT_NAME} example.cpp)

# Link against the QFtpClient library
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        QFtpClient
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
)
