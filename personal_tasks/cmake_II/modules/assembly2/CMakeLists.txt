aux_source_directory(src ASSEMBLY2_LIB_DIR)

find_package(OpenCV REQUIRED)

add_library(assembly2_LIB SHARED ${ASSEMBLY2_LIB_DIR})

target_include_directories(
    assembly2_LIB
    PUBLIC include ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
    assembly2_LIB
    PUBLIC ${OpenCV_LIBS}
)

