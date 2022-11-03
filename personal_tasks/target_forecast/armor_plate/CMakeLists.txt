cmake_minimum_required(VERSION 3.10)

find_package(OpenCV REQUIRED)

project(demo)

add_executable(demo main.cpp src/armor_plate.cpp src/light_bar.cpp)

target_include_directories(
    demo
    PUBLIC include ${OpenCV_INCLUDE_DIRS}
)


target_link_libraries(
    demo
    PUBLIC ${OpenCV_LIBS}
)