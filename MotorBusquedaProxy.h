#ifndef MOTOR_BUSQUEDA_PROXY_H
#define MOTOR_BUSQUEDA_PROXY_H

#include "MotorBusqueda.h"
#include <map>

// ============================================================
// PATRON PROXY
// ============================================================
// MotorBusquedaProxy controla el acceso al MotorBusqueda real.
// En este proyecto se usa como Proxy de cache: si la interfaz o
// las recomendaciones repiten una busqueda, el proxy devuelve el
// resultado guardado sin volver a recorrer el Suffix Tree ni el
// indice invertido.

class MotorBusquedaProxy : public MotorBusqueda {
private:
    MotorBusqueda* motorReal;
    std::map<std::string, std::vector<int>> cacheTitulo;
    std::map<std::string, std::vector<int>> cacheTrama;

public:
    explicit MotorBusquedaProxy(MotorBusqueda* motor);

    std::vector<int> buscarPorTitulo(const std::string& subcadena) override;
    std::vector<int> buscarEnTrama(const std::string& termino) override;
    Pelicula obtenerPelicula(int id) override;
    int getTotalPeliculas() override;
};

#endif
