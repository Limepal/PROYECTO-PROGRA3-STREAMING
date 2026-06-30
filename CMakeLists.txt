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
        InterfazStreaming.cpp   # <-- ¡AÑADE ESTO!
        EstadoInterfaz.h
        EstadosConcretos.h
        # EstadosConcretos.cpp  # <-- Descomenta esto si también tienes un .cpp para los estados
)