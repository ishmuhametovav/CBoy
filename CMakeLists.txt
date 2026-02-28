cmake_minimum_required(VERSION 3.16)

project(CBoy VERSION 0.0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES 
	"src/*.cpp"
)

find_package(OpenGL REQUIRED)

add_subdirectory(dependencies/glad)

add_subdirectory(dependencies/glfw)

add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE dependencies/miniaudio)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE dependencies/glm)

add_library(imgui)
set(IMGUI_DIR dependencies/imgui)
target_sources(imgui PRIVATE
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR})
target_link_libraries(imgui PRIVATE glfw ${OPENGL_LIBRARIES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE 
	miniaudio
	glm
	${OPENGL_LIBRARIES}
	glfw
	glad
	imgui
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
        
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/roms
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/roms
)
