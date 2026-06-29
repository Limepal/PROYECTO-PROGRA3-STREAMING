#ifndef MOTOR_BUSQUEDA_H
#define MOTOR_BUSQUEDA_H

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct Pelicula {
    int id;
    string year, titulo, genero, trama, director, reparto, origen;
};

// ============================================
// NODO DE SUFFIX TREE (Con compresión de aristas)
// ============================================
struct NodoST {
    string etiqueta; // Almacena la subcadena en lugar de un solo char
    vector<NodoST*> hijos;
    vector<int> peliculasIDs;
    // Ya no es estrictamente necesario 'esFin' porque indexamos
    // IDs en todo el camino para acelerar la búsqueda.
};

struct ParPalabraId {
    string palabra;
    int id;
    bool operator<(const ParPalabraId& otro) const {
        if (palabra != otro.palabra) return palabra < otro.palabra;
        return id < otro.id;
    }
};

struct EntradaIndice {
    string palabra;
    vector<int> ids;
};

class MotorBusqueda {
private:
    vector<Pelicula> baseDatos;
    NodoST* raizST; // Renombrado a Suffix Tree
    vector<EntradaIndice> indiceInvertido;
    vector<ParPalabraId> bufferIndexacion;

    // Helper recursivo para la inserción de sufijos
    void insertarSufijo(NodoST* nodo, string sufijo, int id);
    void liberarST(NodoST* nodo);

public:
    MotorBusqueda();
    ~MotorBusqueda();
    void agregarPelicula(const Pelicula& p);

    string normalizarToken(const string &palabra);

    void insertarEnST(const string &titulo, int id); // Reemplaza al Trie
    void indexarTrama(const string &trama, int id);
    void finalizarIndexacion();

    vector<int> buscarPorTitulo(const string& subcadena); // Ahora soporta subcadenas
    vector<int> buscarEnTrama(const string& termino);

    int getTotalPeliculas() { return baseDatos.size(); }
    Pelicula obtenerPelicula(int id);
};

#endif