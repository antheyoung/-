aux_source_directory(src MODULE2_LIB_DIR)

add_library(module2_LIB SHARED ${MODULE2_LIB_DIR})

target_include_directories(
    module2_LIB
    PUBLIC include
)

target_link_libraries(
    module2_LIB
    PUBLIC pthread
)