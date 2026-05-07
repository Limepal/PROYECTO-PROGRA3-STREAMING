#include "MotorBusqueda.h"
#include <algorithm>
#include<vector>
using namespace std;

MotorBusqueda::MotorBusqueda() {
    raizTrie = new NodoTrie();
}

MotorBusqueda::~MotorBusqueda() {
    // Nota: En un sistema real se debería implementar una limpieza
    // recursiva del Trie o usar smart pointers.
}

string MotorBusqueda::normalizarToken(const string& palabra) {
    string resultado;
    for (char c : palabra) {
        if (isalnum((unsigned char)c)) {
            resultado += (char)tolower((unsigned char)c);
        }
    }
    return resultado;
}

void MotorBusqueda::insertarEnTrie(const string& titulo, int id) {
    NodoTrie* actual = raizTrie;
    for (char c : titulo) {
        char n = (char)tolower((unsigned char)c);

        auto it = lower_bound(actual->hijos.begin(), actual->hijos.end(), n,
            [](const pair<char, NodoTrie*>& par, char valor) {
                return par.first < valor;
            });

        if (it != actual->hijos.end() && it->first == n) {
            actual = it->second;
        } else {
            NodoTrie* nuevo = new NodoTrie();
            // Insertar manteniendo el orden alfabético de los hijos
            actual->hijos.insert(it, {n, nuevo});
            actual = nuevo;
        }
    }
    actual->esFin = true;
    actual->peliculasIDs.push_back(id);
}

void MotorBusqueda::indexarTrama(const string& trama, int id) {
    string palabra;
    for (size_t i = 0; i <= trama.length(); ++i) {
        if (i == trama.length() || isspace((unsigned char)trama[i])) {
            if (!palabra.empty()) {
                string limpia = normalizarToken(palabra);
                if (limpia.length() > 2) {
                    // Simplemente guardamos el par, no buscamos nada aún
                    bufferIndexacion.push_back({limpia, id});
                }
                palabra.clear();
            }
        } else {
            palabra += trama[i];
        }
    }
}

void MotorBusqueda::finalizarIndexacion() {
    if (bufferIndexacion.empty()) return;

    // 1. Ordenamos todo el buffer de una sola vez O(N log N)
    sort(bufferIndexacion.begin(), bufferIndexacion.end());

    // 2. Agrupamos palabras repetidas en el índice invertido real
    indiceInvertido.clear();
    for (const auto& par : bufferIndexacion) {
        if (indiceInvertido.empty() || indiceInvertido.back().palabra != par.palabra) {
            indiceInvertido.push_back({par.palabra, {par.id}});
        } else {
            // Evitar meter el mismo ID varias veces para la misma palabra
            if (indiceInvertido.back().ids.back() != par.id) {
                indiceInvertido.back().ids.push_back(par.id);
            }
        }
    }
    // Limpiamos el buffer para liberar memoria
    bufferIndexacion.clear();
    bufferIndexacion.shrink_to_fit();
}



void MotorBusqueda::agregarPelicula(const Pelicula& p) {
    // OPTIMIZACIÓN: Pre-reservar en la base de datos si es la primera vez
    if (baseDatos.empty()) baseDatos.reserve(35000);

    int nuevoId = baseDatos.size();
    baseDatos.push_back(p);

    insertarEnTrie(p.titulo, nuevoId);
    indexarTrama(p.trama, nuevoId);
}

void recolectarIDs(NodoTrie* nodo, vector<int>& resultados) {
    // Agregar los IDs que terminan exactamente en este nodo
    for (int id : nodo->peliculasIDs) {
        resultados.push_back(id);
    }

    // Recorrer todos los hijos recursivamente
    for (auto& par : nodo->hijos) {
        recolectarIDs(par.second, resultados);
    }
}

vector<int> MotorBusqueda::buscarPorTitulo(const string& prefijo) {
    NodoTrie* actual = raizTrie;
    vector<int> resultados;

    // 1. Navegar hasta el nodo que representa el final del prefijo
    for (char c : prefijo) {
        char n = (char)tolower((unsigned char)c);

        // Usamos lower_bound para buscar el caracter en el vector de pares ordenado
        auto it = lower_bound(actual->hijos.begin(), actual->hijos.end(), n,
            [](const pair<char, NodoTrie*>& par, char valor) {
                return par.first < valor;
            });

        if (it != actual->hijos.end() && it->first == n) {
            actual = it->second;
        } else {
            return {}; // El prefijo no existe
        }
    }

    // 2. Una vez en el nodo del prefijo, recolectamos todos los IDs de sus descendientes
    recolectarIDs(actual, resultados);

    // Opcional: Eliminar duplicados si una película aparece bajo el mismo prefijo
    sort(resultados.begin(), resultados.end());
    resultados.erase(unique(resultados.begin(), resultados.end()), resultados.end());

    return resultados;
}


vector<int> MotorBusqueda::buscarEnTrama(const string& termino) {
    string busqueda = normalizarToken(termino);

    // Búsqueda binaria sobre el índice finalizado
    auto it = lower_bound(indiceInvertido.begin(), indiceInvertido.end(), busqueda,
        [](const EntradaIndice& e, const string& val) {
            return e.palabra < val;
        });

    if (it != indiceInvertido.end() && it->palabra == busqueda) {
        return it->ids;
    }
    return {};
}

Pelicula MotorBusqueda::obtenerPelicula(int id) {
    if (id >= 0 && id < baseDatos.size()) {
        return baseDatos[id];
    }
    return Pelicula{-1, "", "", "", ""};
}