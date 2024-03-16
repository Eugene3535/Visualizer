cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(Terrain LANGUAGES C CXX)

set(EXTERNAL_DIR "$ENV{External}")

file(GLOB_RECURSE TERRAIN_SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)

set(TERRAIN_SRC_FILES
	src/graphics/Image.cpp
	src/graphics/Texture2D.cpp
	src/graphics/Shader.cpp
	src/graphics/Vertex.cpp
	src/loaders/MeshLoader.cpp
	src/meshes/Grid.cpp
	src/meshes/ObjectModel.cpp
	src/Camera.cpp
	src/main.cpp
)

add_compile_definitions("$<$<CONFIG:Debug>:DEBUG>")

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
	add_compile_options(-ffast-math)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/fp:fast)
endif()

add_executable(${PROJECT_NAME} ${TERRAIN_SRC_FILES})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(${EXTERNAL_DIR}/glad external/glad)
add_subdirectory(${EXTERNAL_DIR}/glfw external/glfw)
add_subdirectory(${EXTERNAL_DIR}/glm external/glm)

target_link_libraries(${PROJECT_NAME} glfw glad glm)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${EXTERNAL_DIR}/stb"
	"${PROJECT_SOURCE_DIR}/src"
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
	