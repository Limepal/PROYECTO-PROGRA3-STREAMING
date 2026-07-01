cmake_minimum_required(VERSION 3.20)
project(PiliFlix)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(PiliFlix
        main.cpp
        PreprocesamientoDatos.cpp
        PreprocesamientoDatos.h
        MotorBusqueda.cpp
        MotorBusqueda.h
        InterfazStreaming.h
        InterfazStreaming.cpp
        EstadoInterfaz.h
        EstadosConcretos.h
        PerfilMemento.cpp
        PerfilMemento.h
        ObservadorPerfil.h
        EstrategiaRelevancia.h
)
