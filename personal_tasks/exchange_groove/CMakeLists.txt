cmake_minimum_required(VERSION 3.10)

project(demo)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(demo main.cpp src/exchange.cpp src/pnp.cpp)

target_include_directories(
    demo 
    PUBLIC include ${OpenCV_INCLUDE_DIRS} ${Eigen3_INCLUDE_DIRS}
)

target_link_libraries(
    demo
    PUBLIC ${OpenCV_LIBS} ${Eigen3_LIBS}
)

