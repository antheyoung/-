find_package(OpenCV REQUIRED)

aux_source_directory(src/module1a MODULE1A_LIB_DIR)
aux_source_directory(src/module1b MODULE1B_LIB_DIR)

add_library(module1A_LIB SHARED ${MODULE1A_LIB_DIR})
add_library(module1B_LIB SHARED ${MODULE1B_LIB_DIR})


target_include_directories(
    module1A_LIB
    PUBLIC include ${OpenCV_INCLUDE_DIRS} 
)

target_include_directories(
    module1B_LIB
    PUBLIC include ${OpenCV_INCLUDE_DIRS} 
)

target_link_libraries(
    module1A_LIB
    PUBLIC ${OpenCV_LIBS} assembly2_LIB
)

target_link_libraries(
    module1B_LIB
    PUBLIC ${OpenCV_LIBS} assembly2_LIB
)
