cmake_minimum_required(VERSION 3.17)
project(SoftRenderer)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(SDL2_PATH "libs\\SDL2-2.0.20\\x86_64-w64-mingw32")
set(HEADER
	include/window.h include/camera.h include/vector.h include/matrix.h include/math_util.h
	include/pipeline.h include/shader.h include/frame_buffer.h
	include/mesh.h include/texture.h include/vertex.h include/light.h include/scene.h include/aabb.h include/shadow_map.h include/global_config.h include/skybox.h)
set(SOURCE
	src/main.cpp src/window.cpp src/camera.cpp src/pipeline.cpp
	src/shader.cpp src/frame_buffer.cpp src/mesh.cpp src/texture.cpp src/light.cpp src/scene.cpp src/aabb.cpp src/shadow_map.cpp src/skybox.cpp)

find_package(SDL2 REQUIRED)

add_executable(SoftRenderer ${SOURCE} ${HEADER})
target_include_directories(SoftRenderer
	PRIVATE ${SDL2_INCLUDE_DIR}
	PRIVATE ${PROJECT_SOURCE_DIR}/include
	PRIVATE ${PROJECT_SOURCE_DIR}/libs
	)
target_link_libraries(SoftRenderer
	PRIVATE ${SDL2_LIBRARY}
	)

#if (WIN32)
#  add_custom_command(TARGET SoftRenderer POST_BUILD COMMAND
#	  ${CMAKE_COMMAND} -E copy_if_different
#	  ${SDL2_DLL}
#	  $<TARGET_FILE_DIR:SoftRenderer>)
#endif ()