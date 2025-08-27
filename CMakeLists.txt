cmake_minimum_required(VERSION 4.0 FATAL_ERROR)


project(ImageLib VERSION 0.0.1 LANGUAGES C)

# Основные настройки для cmake
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}\\bin)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# файлы игры
set(FILES 
    src/image.c
    src/wrapSTB.c
    src/atlas.c
    )

add_library(${PROJECT_NAME} STATIC ${FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)