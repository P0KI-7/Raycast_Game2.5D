cmake_minimum_required(VERSION 3.16)
project(SFML_25D_Raycast_Shooter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Генерация json файлов
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(sfml_25d_shooter ${GAME_SOURCES})
target_include_directories(sfml_25d_shooter PRIVATE include)
target_link_libraries(sfml_25d_shooter PRIVATE sfml-graphics sfml-window sfml-system)

if (MSVC)
    target_compile_options(sfml_25d_shooter PRIVATE /W4 /permissive-)
else()
    target_compile_options(sfml_25d_shooter PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(TARGET sfml_25d_shooter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:sfml_25d_shooter>/assets
    COMMENT "Copying assets to build directory"
)