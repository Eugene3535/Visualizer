cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(Visualizer LANGUAGES C CXX)

set(EXTERNAL_DIR "$ENV{External}")
set(THIRD_PARTY_LIB_DIR "${EXTERNAL_DIR}/install")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_compile_definitions("$<$<CONFIG:Debug>:DEBUG>")

set(VISUALIZER_SRC_FILES
	src/graphics/Image.cpp
	src/graphics/Texture2D.cpp
	src/graphics/Shader.cpp
	src/graphics/VertexBuffer.cpp
	src/loaders/MeshLoader.cpp
	src/view/Camera.cpp
	src/main.cpp
)

set(VISUALIZER_HDR_FILES
	src/graphics/Image.hpp
	src/graphics/Texture2D.hpp
	src/graphics/Shader.hpp
	src/graphics/Vertex.hpp
	src/graphics/VertexBuffer.hpp
	src/loaders/MeshLoader.hpp
	src/view/Camera.hpp
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

	set(VISUALIZER_ALL_FILES 
		${VISUALIZER_SRC_FILES} 
		${VISUALIZER_HDR_FILES})

	foreach(source IN LISTS VISUALIZER_ALL_FILES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "/" "\\" source_path_msvc "${source_path}")
		source_group("${source_path_msvc}" FILES "${source}")
	endforeach()

endif()

add_executable(${PROJECT_NAME} ${VISUALIZER_SRC_FILES} ${VISUALIZER_HDR_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLAD_GLAPI_EXPORT)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${THIRD_PARTY_LIB_DIR}/glfw/include"
	"${THIRD_PARTY_LIB_DIR}/glad/include"
	"${THIRD_PARTY_LIB_DIR}/glm/include"
	"${THIRD_PARTY_LIB_DIR}/assimp/include"
	"${EXTERNAL_DIR}/stb"
	"${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
	"${THIRD_PARTY_LIB_DIR}/glfw/lib/glfw3$<$<CONFIG:Debug>:d>dll.lib"
	"${THIRD_PARTY_LIB_DIR}/glad/lib/glad$<$<CONFIG:Debug>:d>.lib"
	"${THIRD_PARTY_LIB_DIR}/glm/lib/glm$<$<CONFIG:Debug>:d>.lib"
	"${THIRD_PARTY_LIB_DIR}/assimp/lib/assimp-vc142-mt$<$<CONFIG:Debug>:d>.lib"
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_DIR}/install/glfw/bin/glfw3$<$<CONFIG:Debug>:d>.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3$<$<CONFIG:Debug>:d>.dll
        COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_DIR}/install/glad/bin/glad$<$<CONFIG:Debug>:d>.dll  $<TARGET_FILE_DIR:${PROJECT_NAME}>/glad$<$<CONFIG:Debug>:d>.dll
        COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_DIR}/install/glm/bin/glm$<$<CONFIG:Debug>:d>.dll    $<TARGET_FILE_DIR:${PROJECT_NAME}>/glm$<$<CONFIG:Debug>:d>.dll
        COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_DIR}/install/assimp/bin/assimp-vc142-mt$<$<CONFIG:Debug>:d>.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>/assimp-vc142-mt$<$<CONFIG:Debug>:d>.dll
    )
endif(WIN32)
	