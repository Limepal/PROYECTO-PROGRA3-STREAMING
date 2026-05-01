#include <iostream>
#include "PreprocesamientoDatos.h"

int main() {
    std::cout << "Iniciando proceso de limpieza..." << std::endl;

    // Llamada a la función del módulo limpiezaDatos.cpp
    LimpiarDatos("wiki_movie_plots_deduped.csv", "datos_listos.csv");

    std::cout << "Proceso finalizado. Archivo 'datos_listos.csv' creado." << std::endl;

    return 0;
}