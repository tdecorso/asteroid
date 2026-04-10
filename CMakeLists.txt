cmake_minimum_required(VERSION 3.31)

project(Asteroid)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRCS
    src/platform.c
    src/audio.c
    src/framebuffer.c
    src/rasterizer.c
    src/texture.c
    src/quad2D.c
    src/transform2D.c
    src/projection.c
    src/camera2D.c
    src/engine.c
    src/main.c)

add_executable(Asteroid ${SRCS})

target_link_libraries(Asteroid PRIVATE SDL3 m)
