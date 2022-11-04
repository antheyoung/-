cmake_minimum_required(VERSION 3.10)

project(Test)

# # # # 以下内容禁止修改 # # # #
option(BUILD_A "build modules a" ON)
option(BUILD_B "build modules b" OFF)
option(BUILD_TESTS "build tests" OFF)

if(BUILD_TESTS)
enable_testing()
endif()

add_subdirectory(modules)
add_subdirectory(common)
# 小提示: module2 需要用到 pthread，即 target_link_libraries(xxx pthread)
# # # # 以上内容禁止修改 # # # #
if(BUILD_A)
    add_compile_definitions(WITH_A)
elseif(BUILD_B)
    add_compile_definitions(WITH_B)
endif()


find_package(OpenCV REQUIRED)

add_executable(server server.cpp)

add_executable(client client.cpp)


list(APPEND CMAKE_CXX_FLAGS "-fPIC")

target_include_directories(
    server
    PUBLIC ${OpenCV_INCLUDE_DIRS} common/singleton/include
)


target_link_libraries(
    server
    PUBLIC 
    ${OpenCV_LIBS} assembly1_LIB assembly2_LIB 
    module1A_LIB 
    module1B_LIB 
    module2_LIB rmath_LIB singleton_LIB
)

target_link_libraries(
    client
    PUBLIC ${OpenCV_LIBS} module1A_LIB module1B_LIB module2_LIB rmath_LIB singleton_LIB
)