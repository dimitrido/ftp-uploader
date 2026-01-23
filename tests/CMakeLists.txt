# Enable testing
enable_testing()

# Find Qt Test module
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

# Individual test executables
add_executable(test_ftperror test_ftperror.cpp)
target_link_libraries(test_ftperror
    PRIVATE QFtpClient Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_include_directories(test_ftperror PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME TestFtpError COMMAND test_ftperror)

add_executable(test_ftpnetworkreply test_ftpnetworkreply.cpp)
target_link_libraries(test_ftpnetworkreply
    PRIVATE QFtpClient Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_include_directories(test_ftpnetworkreply PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME TestFtpNetworkReply COMMAND test_ftpnetworkreply)

add_executable(test_ftpnetworkaccessmanager test_ftpnetworkaccessmanager.cpp)
target_link_libraries(test_ftpnetworkaccessmanager
    PRIVATE QFtpClient Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_include_directories(test_ftpnetworkaccessmanager PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME TestFtpNetworkAccessManager COMMAND test_ftpnetworkaccessmanager)
