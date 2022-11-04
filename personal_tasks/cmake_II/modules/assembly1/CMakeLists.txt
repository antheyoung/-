aux_source_directory(src ASSEMBLY1_LIB_DIR)

add_library(assembly1_LIB SHARED ${ASSEMBLY1_LIB_DIR})

target_include_directories(
    assembly1_LIB
    PUBLIC include
)

