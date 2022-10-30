# OpenCV
set (OpenCV_DIR ${CMAKE_SOURCE_DIR}/3rdparty/opencv-arm-linux/lib/cmake/opencv4)
find_package(OpenCV REQUIRED)

# Conan libs
# include这个文件以引入三方库
set(CONAN_BUILD_INFO "${CMAKE_SOURCE_DIR}/3rdparty/conanbuildinfo.cmake")
if(EXISTS "${CONAN_BUILD_INFO}")
    include(${CONAN_BUILD_INFO})
    conan_basic_setup()
else()
    message(FATAL_ERROR " ${CONAN_BUILD_INFO} DOES NOT EXIST, please run 'conan install .. --build --profile:host=../conan_profile/ax620")
endif()