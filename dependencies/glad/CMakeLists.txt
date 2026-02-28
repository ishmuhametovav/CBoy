cmake_minimum_required(VERSION 3.16)
project(glad VERSION 2.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c
)
	
target_include_directories(${PROJECT_NAME} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)