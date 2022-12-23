project(ImageToSketchCLITool)

find_package(gflags REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} gflags ${OpenCV_LIBS} imageSDK::SketchLib)