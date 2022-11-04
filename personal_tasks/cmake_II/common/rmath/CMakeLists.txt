find_package(OpenCV REQUIRED)

aux_source_directory(src RMATH_LIB_DIR)

add_library(rmath_LIB SHARED ${RMATH_LIB_DIR})

target_include_directories(
    rmath_LIB
    PUBLIC include ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(
    rmath_LIB
    PUBLIC ${OpenCV_LIBS}
)