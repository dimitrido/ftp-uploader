cmake_minimum_required(VERSION 3.10)
# Find libcurl
find_package(CURL REQUIRED)

# Library sources
set(SOURCES
    src/ftpnetworkaccessmanager.cpp
    src/ftpnetworkreply.cpp
)

set(HEADERS
    include/FtpClient/ftpnetworkaccessmanager.h
    include/FtpClient/ftpnetworkreply.h
    include/FtpClient/FtpClient
)

# Create the library
add_library(ftp-uploader ${SOURCES} ${HEADERS})

# Set include directories
target_include_directories(ftp-uploader
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CURL_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(ftp-uploader
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
    PRIVATE
        ${CURL_LIBRARIES}
)

target_compile_options(ftp-uploader PRIVATE -fPIC)

# Set target properties
set_target_properties(ftp-uploader PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${HEADERS}"
)

# End of ftp-uploader CMakeLists.txt
