cmake_minimum_required(VERSION 3.20)
project(PROYECTO_PROGRAMACION)

set(CMAKE_CXX_STANDARD 20)

add_executable(PROYECTO_PROGRAMACION
        main.cpp
        PreprocesamientoDatos.cpp
        PreprocesamientoDatos.h
        MotorBusqueda.cpp
        MotorBusqueda.h
        InterfazStreaming.h
        EstadoInterfaz.h
        EstadosConcretos.h
)