add_executable(cjp2)

target_sources(cjp2 PRIVATE main.cpp)

add_subdirectory(lib_cjp2)
target_link_libraries(cjp2 PRIVATE lib_cjp2)
target_include_directories(cjp2 PRIVATE "src/lib_cjp2")
