cmake_minimum_required(VERSION 3.17)

project(
	jpeg2000
	VERSION 0.1.0
	LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(src)

if(ENABLE_INSTALL)
	install(
		TARGETS cjp2
		EXPORT ${PROJECT_NAME}_Targets
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
