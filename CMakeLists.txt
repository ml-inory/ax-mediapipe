cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(ax_mediapipe)

enable_language(C CXX)

# ======== 引入三方库 ========
include(${CMAKE_SOURCE_DIR}/cmake/import_3rdparty.cmake)
set (BSP_MSP_DIR ${CMAKE_SOURCE_DIR}/3rdparty/ax_bsp)
message(STATUS "BSP_MSP_DIR: ${BSP_MSP_DIR}")

include_directories(include)
aux_source_directory(src SRC)
add_executable(${PROJECT_NAME} main.cpp ${SRC})

target_include_directories (${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

target_include_directories (${PROJECT_NAME} PRIVATE ${BSP_MSP_DIR}/include)
target_include_directories (${PROJECT_NAME} PRIVATE ${BSP_MSP_DIR}/include/npu_cv_kit)
target_link_directories (${PROJECT_NAME} PRIVATE ${BSP_MSP_DIR}/lib)
target_link_libraries (${PROJECT_NAME} PRIVATE pthread dl) # ax620a use this
target_link_libraries (${PROJECT_NAME} PRIVATE ax_run_joint ax_interpreter_external ax_interpreter ax_sys axsyslog stdc++fs)

target_link_libraries (${PROJECT_NAME} PRIVATE ${CONAN_LIBS})

install (TARGETS ${PROJECT_NAME} DESTINATION bin)