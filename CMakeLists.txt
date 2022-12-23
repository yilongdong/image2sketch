cmake_minimum_required(VERSION 3.23)
project(ImageToSketchCore
        VERSION 0.1
        LANGUAGES CXX
        DESCRIPTION "一个借助openCV将图片转为素描画的程序")

# cmake通用配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 配置文件
set(RESOURCE ${PROJECT_SOURCE_DIR}/resource)
set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(AUTHOR yilongdong)
configure_file(include/config.h.in ${PROJECT_SOURCE_DIR}/include/config.h @ONLY)

# 三方库
## openCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})

include_directories(include)
add_subdirectory(src)
add_subdirectory(standalone)
add_subdirectory(test)
