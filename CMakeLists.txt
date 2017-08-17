cmake_minimum_required(VERSION 3.7)
project(mvis)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/include/freetype2/)

set(SOURCE_FILES main.cpp Lorenz.h Lorenz.cpp FifoAudioSource.cpp FifoAudioSource.h AudioSource.h)

add_executable(mvis ${SOURCE_FILES})

target_link_libraries(mvis GL GLEW glfw assimp SOIL freetype k5)

add_custom_command(TARGET mvis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:mvis>/shaders)

add_custom_command(TARGET mvis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:mvis>/textures)

add_custom_command(TARGET mvis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/fonts
        $<TARGET_FILE_DIR:mvis>/fonts)