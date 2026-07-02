#include "MotorBusqueda.h"
#include <cctype>

using namespace std;

MotorBusqueda::MotorBusqueda() {
    raizST = new NodoST();
}

void MotorBusqueda::liberarST(NodoST* nodo) {
    if (!nodo) return;
    for (NodoST* hijo : nodo->hijos) {
        liberarST(hijo);
    }
    delete nodo;
}

MotorBusqueda::~MotorBusqueda() {
    liberarST(raizST);
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

// ============================================
// CONSTRUCCIÓN DEL SUFFIX TREE
// ============================================

void MotorBusqueda::insertarSufijo(NodoST* nodo, string suf, int id) {
    // Agregamos el ID al nodo por el que estamos pasando (evitando duplicados)
    if (nodo->peliculasIDs.empty() || nodo->peliculasIDs.back() != id) {
        nodo->peliculasIDs.push_back(id);
    }

    if (suf.empty()) return;

    // Buscamos un hijo que empiece con el primer caracter del sufijo
    NodoST* hijoCoincidente = nullptr;
    int indexHijo = -1;
    for (size_t i = 0; i < nodo->hijos.size(); ++i) {
        if (nodo->hijos[i]->etiqueta[0] == suf[0]) {
            hijoCoincidente = nodo->hijos[i];
            indexHijo = i;
            break;
        }
    }

    // Caso A: No hay coincidencias. Creamos una nueva arista (hoja)
    if (!hijoCoincidente) {
        NodoST* nuevo = new NodoST();
        nuevo->etiqueta = suf;
        nuevo->peliculasIDs.push_back(id);

        // Inserción ordenada para búsquedas rápidas después
        auto it = upper_bound(nodo->hijos.begin(), nodo->hijos.end(), nuevo,
            [](NodoST* a, NodoST* b) { return a->etiqueta[0] < b->etiqueta[0]; });
        nodo->hijos.insert(it, nuevo);
        return;
    }

    // Caso B: Hay coincidencia. Encontramos el prefijo común
    string arista = hijoCoincidente->etiqueta;
    size_t j = 0;
    while (j < arista.length() && j < suf.length() && arista[j] == suf[j]) {
        j++;
    }

    // Subcaso B1: La arista es prefijo completo del sufijo. Bajamos al siguiente nodo.
    if (j == arista.length()) {
        insertarSufijo(hijoCoincidente, suf.substr(j), id);
    }
    // Subcaso B2: DIVISIÓN DE ARISTA (Edge Split / Compresión de Suffix Tree)
    else {
        NodoST* nodoIntermedio = new NodoST();
        nodoIntermedio->etiqueta = arista.substr(0, j);

        // El intermedio hereda todos los IDs del hijo original
        nodoIntermedio->peliculasIDs = hijoCoincidente->peliculasIDs;
        // Y añadimos el ID actual si no está
        if (nodoIntermedio->peliculasIDs.empty() || nodoIntermedio->peliculasIDs.back() != id) {
            nodoIntermedio->peliculasIDs.push_back(id);
        }

        // Acortamos la etiqueta del hijo original y lo colgamos del intermedio
        hijoCoincidente->etiqueta = arista.substr(j);
        nodoIntermedio->hijos.push_back(hijoCoincidente);

        // Reemplazamos el hijo original en el nodo padre
        nodo->hijos[indexHijo] = nodoIntermedio;

        // Si sobra parte del sufijo, lo añadimos como nuevo hijo del intermedio
        if (j < suf.length()) {
            NodoST* nuevaHoja = new NodoST();
            nuevaHoja->etiqueta = suf.substr(j);
            nuevaHoja->peliculasIDs.push_back(id);

            // Mantener el orden de los hijos en el intermedio
            if (nuevaHoja->etiqueta[0] < hijoCoincidente->etiqueta[0]) {
                nodoIntermedio->hijos.insert(nodoIntermedio->hijos.begin(), nuevaHoja);
            } else {
                nodoIntermedio->hijos.push_back(nuevaHoja);
            }
        }
    }
}

void MotorBusqueda::insertarEnST(const string& titulo, int id) {
    string titLimpio = normalizarToken(titulo);
    // Añadimos un símbolo terminador para diferenciar hojas de prefijos
    titLimpio += "$";

    // MAGIA DEL SUFFIX TREE: Insertamos todos y cada uno de los sufijos
    for (size_t i = 0; i < titLimpio.length(); ++i) {
        insertarSufijo(raizST, titLimpio.substr(i), id);
    }
}

// ============================================
// BÚSQUEDA EN SUFFIX TREE (Subcadenas)
// ============================================

vector<int> MotorBusqueda::buscarPorTitulo(const string& subcadena) {
    string query = normalizarToken(subcadena);
    if (query.empty()) return {};

    NodoST* actual = raizST;
    size_t i = 0; // Puntero de lectura en la query

    while (i < query.length()) {
        NodoST* hijoCoincidente = nullptr;

        // Buscar hijo que empiece con el caracter actual
        for (NodoST* hijo : actual->hijos) {
            if (hijo->etiqueta[0] == query[i]) {
                hijoCoincidente = hijo;
                break;
            }
        }

        if (!hijoCoincidente) return {}; // La subcadena no existe

        string arista = hijoCoincidente->etiqueta;
        size_t j = 0;

        // Comparar query con el texto de la arista comprimida
        while (j < arista.length() && i < query.length() && arista[j] == query[i]) {
            j++;
            i++;
        }

        if (i == query.length()) {
            // Encontramos el término de búsqueda completo (esté o no a mitad de arista).
            // Retornamos los IDs cacheados (ya vienen ordenados y sin duplicados de id).
            return hijoCoincidente->peliculasIDs;
        }

        if (j < arista.length()) {
            // Divergencia antes de terminar la query -> No existe en el árbol
            return {};
        }

        // Si la arista se consumió completa pero la query no, avanzamos al hijo
        actual = hijoCoincidente;
    }
    return {};
}

// ============================================
// RESTO DEL CÓDIGO (Sin cambios mayores)
// ============================================

void MotorBusqueda::agregarPelicula(const Pelicula& p) {
    if (baseDatos.empty()) baseDatos.reserve(35000);

    int nuevoId = baseDatos.size();
    Pelicula estable = p;
    estable.id = nuevoId;
    baseDatos.push_back(estable);

    insertarEnST(p.titulo, nuevoId);
    indexarTrama(p.trama, nuevoId);
}

void MotorBusqueda::tokenizarYAgregar(const string& texto, int id, vector<ParPalabraId>& buffer) {
    string palabra;
    for (size_t i = 0; i <= texto.length(); ++i) {
        if (i == texto.length() || isspace((unsigned char)texto[i])) {
            if (!palabra.empty()) {
                string limpia = normalizarToken(palabra);
                if (limpia.length() > 2) {
                    buffer.push_back({limpia, id});
                }
                palabra.clear();
            }
        } else {
            palabra += texto[i];
        }
    }
}

void MotorBusqueda::indexarTrama(const string& trama, int id) {
    tokenizarYAgregar(trama, id, bufferIndexacion);
}

void MotorBusqueda::finalizarIndexacion() {
    if (bufferIndexacion.empty()) return;

    sort(bufferIndexacion.begin(), bufferIndexacion.end());

    indiceInvertido.clear();
    for (const auto& par : bufferIndexacion) {
        if (indiceInvertido.empty() || indiceInvertido.back().palabra != par.palabra) {
            indiceInvertido.push_back({par.palabra, {par.id}});
        } else {
            if (indiceInvertido.back().ids.back() != par.id) {
                indiceInvertido.back().ids.push_back(par.id);
            }
        }
    }
    bufferIndexacion.clear();
    bufferIndexacion.shrink_to_fit();
}

vector<int> MotorBusqueda::buscarEnTrama(const string& termino) {
    string busqueda = normalizarToken(termino);
    if (busqueda.empty()) return {};

    // Camino rapido para palabras completas.
    auto it = lower_bound(indiceInvertido.begin(), indiceInvertido.end(), busqueda,
        [](const EntradaIndice& e, const string& val) {
            return e.palabra < val;
        });

    // El enunciado tambien exige sub-palabras en la sinopsis ("bar" en
    // "barco"). El indice sigue evitando recorrer todas las peliculas:
    // recorremos el vocabulario unico y unimos sus listas de IDs.
    vector<int> resultados;
    if (it != indiceInvertido.end() && it->palabra == busqueda)
        resultados = it->ids;
    for (const auto& entrada : indiceInvertido) {
        if (entrada.palabra != busqueda &&
            entrada.palabra.find(busqueda) != string::npos) {
            resultados.insert(resultados.end(), entrada.ids.begin(), entrada.ids.end());
        }
    }
    sort(resultados.begin(), resultados.end());
    resultados.erase(unique(resultados.begin(), resultados.end()), resultados.end());
    return resultados;
}

vector<int> MotorBusqueda::buscarPorTag(const string& campo, const string& termino) {
    string consulta = normalizarToken(termino);
    if (consulta.empty()) return {};

    vector<int> resultados;
    for (const Pelicula& p : baseDatos) {
        const string* valor = nullptr;
        if (campo == "director") valor = &p.director;
        else if (campo == "reparto" || campo == "casting" || campo == "cast") valor = &p.reparto;
        else if (campo == "genero" || campo == "genre") valor = &p.genero;
        else if (campo == "origen" || campo == "origin") valor = &p.origen;
        else if (campo == "year" || campo == "anio") valor = &p.year;
        if (valor && normalizarToken(*valor).find(consulta) != string::npos) {
            resultados.push_back(p.id);
        }
    }
    return resultados;
}

Pelicula MotorBusqueda::obtenerPelicula(int id) {
    if (id >= 0 && id < baseDatos.size()) {
        return baseDatos[id];
    }
        return Pelicula{-1, "", "", "", "", "", "", ""};
}
