//
// Created by gabri on 1/05/2026.
//

#ifndef PROYECTO_PROGRAMACION_PREPROCESAMIENTODATOS_H
#define PROYECTO_PROGRAMACION_PREPROCESAMIENTODATOS_H

#include <string>
#include<vector>
using namespace std;
// Prototipo de la función
#include <string>
#include <vector>

struct DatosPelicula {
    string year, titulo, origen, director, reparto, genero, trama;
};

void LimpiarDatos(std::string nombreEntrada, std::string nombreSalida);
// Nueva función para obtener los datos directamente en memoria
    vector<DatosPelicula> CargarYLimpiarDatos(std::string nombreArchivo);

#endif //PROYECTO_PROGRAMACION_PREPROCESAMIENTODATOS_H