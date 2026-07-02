#ifndef MOTOR_BUSQUEDA_H
#define MOTOR_BUSQUEDA_H

#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

struct Pelicula {
    int id;
    string year, titulo, genero, trama, director, reparto, origen;
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

struct EntradaIndice {
    string palabra;
    vector<int> ids;
};

class MotorBusqueda {
private:
    vector<Pelicula> baseDatos;
    vector<NodoST*> raicesST;
    vector<EntradaIndice> indiceInvertido;
    vector<ParPalabraId> bufferIndexacion;

    // Helper recursivo para la inserción de sufijos
    void insertarSufijo(NodoST* nodo, string sufijo, int id);
    void liberarST(NodoST* nodo);
    void tokenizarYAgregar(const string& texto, int id, vector<ParPalabraId>& buffer);
    vector<int> buscarEnRaiz(NodoST* raiz, const string& query);

public:
    MotorBusqueda();
    ~MotorBusqueda();
    void agregarPelicula(const Pelicula& p);
    void prepararCarga(size_t cantidad, size_t cantidadArboles);
    void agregarPeliculaConcurrente(const Pelicula& p, int id,
                                    size_t indiceArbol,
                                    vector<ParPalabraId>& bufferLocal);
    void mergeBuffers(vector<vector<ParPalabraId>>& buffersLocales);
    void finalizarIndexacionParalela(vector<vector<ParPalabraId>>& buffersLocales);

    string normalizarToken(const string &palabra);

    void insertarEnST(const string &titulo, int id); // Reemplaza al Trie
    void indexarTrama(const string &trama, int id);
    void finalizarIndexacion();

    vector<int> buscarPorTitulo(const string& subcadena); // Ahora soporta subcadenas
    vector<int> buscarEnTrama(const string& termino);
    vector<int> buscarPorTag(const string& campo, const string& termino);

    int getTotalPeliculas() { return baseDatos.size(); }
    Pelicula obtenerPelicula(int id);
};

#endif
