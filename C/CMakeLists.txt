cmake_minimum_required(VERSION 4.1)
project(ChaseHQ C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")

find_package(SDL2 REQUIRED)

set(SOURCE_FILES
        Main.c
        ChaseHQ/ChaseHQ.c
        ChaseHQ/ChaseHQ-Data.c
        ChaseHQ/ChaseHQ-Stages.c
        ChaseHQ/ChaseHQ-Stage1.c
        ChaseHQ/ChaseHQ-Stage1.h
        ZXSpectrum/Screen.c)

add_executable(ChaseHQ ${SOURCE_FILES})
target_compile_definitions(ChaseHQ PRIVATE TGE_SDL)
target_include_directories(ChaseHQ PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(ChaseHQ ${SDL2_LIBRARIES})
