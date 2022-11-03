cmake_minimum_required(VERSION 3.10)

find_package(OpenCV REQUIRED)

project(demo)

add_executable(demo main.cpp src/energy_m.cpp)

target_include_directories(
    demo
    PUBLIC include ${OpenCV_INCLUDE_DIRS}  
)


target_link_libraries(
    demo
    PUBLIC ${OpenCV_LIBS}
)