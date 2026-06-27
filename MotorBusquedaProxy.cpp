#include "MotorBusquedaProxy.h"

MotorBusquedaProxy::MotorBusquedaProxy(MotorBusqueda* motor) : motorReal(motor) {}

std::vector<int> MotorBusquedaProxy::buscarPorTitulo(const std::string& subcadena) {
    std::string clave = normalizarToken(subcadena);

    auto it = cacheTitulo.find(clave);
    if (it != cacheTitulo.end()) {
        return it->second;
    }

    std::vector<int> resultado = motorReal->buscarPorTitulo(subcadena);
    cacheTitulo[clave] = resultado;
    return resultado;
}

std::vector<int> MotorBusquedaProxy::buscarEnTrama(const std::string& termino) {
    std::string clave = normalizarToken(termino);

    auto it = cacheTrama.find(clave);
    if (it != cacheTrama.end()) {
        return it->second;
    }

    std::vector<int> resultado = motorReal->buscarEnTrama(termino);
    cacheTrama[clave] = resultado;
    return resultado;
}

Pelicula MotorBusquedaProxy::obtenerPelicula(int id) {
    return motorReal->obtenerPelicula(id);
}

int MotorBusquedaProxy::getTotalPeliculas() {
    return motorReal->getTotalPeliculas();
}
