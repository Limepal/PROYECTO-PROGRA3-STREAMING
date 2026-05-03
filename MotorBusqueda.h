#ifndef MOTOR_BUSQUEDA_H
#define MOTOR_BUSQUEDA_H

#include <string>
#include <vector>
#include <algorithm>

struct Pelicula {
    int id;
    std::string year, titulo, genero, trama;
};

struct NodoTrie {
    std::vector<std::pair<char, NodoTrie*>> hijos;
    std::vector<int> peliculasIDs;
    bool esFin = false;
};

struct ParPalabraId {
    std::string palabra;
    int id;
    // Operador para ordenar alfabéticamente
    bool operator<(const ParPalabraId& otro) const {
        if (palabra != otro.palabra) return palabra < otro.palabra;
        return id < otro.id;
    }
};

// Estructura para el índice invertido basado en vectores
struct EntradaIndice {
    std::string palabra;
    std::vector<int> ids;
};

class MotorBusqueda {
private:
    std::vector<Pelicula> baseDatos;
    NodoTrie* raizTrie;
    std::vector<EntradaIndice> indiceInvertido;
    std::vector<ParPalabraId> bufferIndexacion; // Buffer temporal

public:
    MotorBusqueda();
    ~MotorBusqueda();
    void agregarPelicula(const Pelicula& p);

    std::string normalizarToken(const std::string &palabra);

    void insertarEnTrie(const std::string &titulo, int id);

    void indexarTrama(const std::string &trama, int id);

    void finalizarIndexacion(); // ESTA FUNCIÓN ES CRÍTICA AHORA
    std::vector<int> buscarPorTitulo(const std::string& prefijo);
    std::vector<int> buscarEnTrama(const std::string& termino);
    Pelicula obtenerPelicula(int id);
};

#endif