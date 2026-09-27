add_compile_options()
add_executable(bin_mold_header ./main.cpp)

target_include_directories(bin_mold_header
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)
