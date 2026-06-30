cmake_minimum_required(VERSION 3.20)
project(PROYECTO_PROGRAMACION LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(PROYECTO_PROGRAMACION main.cpp
        PreprocesamientoDatos.cpp
        PreprocesamientoDatos.h
        MotorBusqueda.cpp
        MotorBusqueda.h
        InterfazStreaming.h
        InterfazStreaming.cpp)

target_compile_definitions(PROYECTO_PROGRAMACION PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_custom_command(TARGET PROYECTO_PROGRAMACION POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/wiki_movie_plots_deduped.csv"
        "$<TARGET_FILE_DIR:PROYECTO_PROGRAMACION>/wiki_movie_plots_deduped.csv")