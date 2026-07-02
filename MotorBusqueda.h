#ifndef MOTOR_BUSQUEDA_H
#define MOTOR_BUSQUEDA_H

#include <string>
#include <vector>
#include <algorithm>
#include <mutex>

using namespace std;

struct Pelicula {
    int id;
    int year;
    string titulo, genero, trama, director, reparto, origen;
};

// NODO DE SUFFIX TREE
struct NodoST {
    string etiqueta; // Almacena la subcadena en lugar de un solo char
    vector<NodoST*> hijos;
    vector<int> peliculasIDs;
    // Ya no es estrictamente necesario 'esFin' porque indexamos
    // IDs en tod el camino para acelerar la búsqueda.
};

struct ParPalabraId {
    string palabra;
    int id;
    bool operator<(const ParPalabraId& otro) const {
        if (palabra != otro.palabra) return palabra < otro.palabra;
        return id < otro.id;
    }
};

//PROGRA GENERICA OU YEA

template <typename T>
struct EntradaIndice {
    T clave;
    vector<int> ids;
};

template <typename T>
vector<int> buscarEnIndiceGenerico(const vector<EntradaIndice<T>>& indice, const T& busqueda);
template <typename T>
void construirIndice(const vector<pair<T, int>>& buffer, vector<EntradaIndice<T>>& indice);

class MotorBusqueda {
private:
    vector<Pelicula> baseDatos;
    NodoST* raizST;
    vector<EntradaIndice<string>> indiceInvertido;
    vector<ParPalabraId> bufferIndexacion;
    mutex mtxST;

    // Helper recursivo para la inserción de sufijos
    void insertarSufijo(NodoST* nodo, string sufijo, int id);
    void liberarST(NodoST* nodo);
    void tokenizarYAgregar(const string& texto, int id, vector<ParPalabraId>& buffer);


    vector<EntradaIndice<int>> indiceAnios;
    vector<pair<int, int>> bufferAnios;
public:
    MotorBusqueda();
    ~MotorBusqueda();
    void agregarPelicula(const Pelicula& p);
    void agregarPeliculaConcurrente(const Pelicula& p, vector<ParPalabraId>& bufferLocal);
    void mergeBuffers(const vector<vector<ParPalabraId>>& buffersLocales);

    string normalizarToken(const string &palabra);

    void insertarEnST(const string &titulo, int id); // Reemplaza al Trie
    void indexarTrama(const string &trama, int id);
    void finalizarIndexacion();

    vector<int> buscarPorTitulo(const string& subcadena); // Ahora soporta subcadenas
    vector<int> buscarEnTrama(const string& termino);

    int getTotalPeliculas() { return baseDatos.size(); }
    Pelicula obtenerPelicula(int id);

    vector<int> buscarPorAnio(int anio);
};

#endif