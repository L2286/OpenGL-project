cmake_minimum_required(VERSION 3.15)
project(first_prj C)

set(C_STANDARD 23)
set(C_STANDARD_REQUIRED ON)

# Источники
file(GLOB PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/main.c"
    "${CMAKE_SOURCE_DIR}/include/*.c"
)

add_executable(first_prj WIN32 ${PROJECT_SOURCES})

# include-пути
target_include_directories(first_prj PRIVATE
    "${CMAKE_SOURCE_DIR}/headers"
    "${CMAKE_SOURCE_DIR}/stb-master"
)

# Линковка (Windows)
target_link_libraries(first_prj PRIVATE
    opengl32
    glu32
    gdi32
)

# Выходные папки (Debug/Release)
set_target_properties(first_prj PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
)


# Включаем предупреждения
if(MSVC)
    target_compile_options(first_prj PRIVATE /W4)
else()
    target_compile_options(first_prj PRIVATE -Wall -Wextra)
endif()