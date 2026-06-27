// PreprocesamientoDatos.h

#ifndef PROYECTO_PROGRAMACION_PREPROCESAMIENTODATOS_H
#define PROYECTO_PROGRAMACION_PREPROCESAMIENTODATOS_H

#include <string>
#include <vector>

struct DatosPelicula {
    std::string year;
    std::string titulo;
    std::string origen;
    std::string director;
    std::string reparto;
    std::string genero;
    std::string trama;
};

// Limpia y genera un nuevo CSV procesado
void LimpiarDatos(const std::string& nombreEntrada,
                  const std::string& nombreSalida);

// Carga datos ya limpios desde CSV
std::vector<DatosPelicula> CargarDatosLimpios(
    const std::string& nombreArchivo
);

#endif //PROYECTO_PROGRAMACION_PREPROCESAMIENTODATOS_H