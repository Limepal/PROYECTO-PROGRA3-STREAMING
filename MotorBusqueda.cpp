#include "MotorBusqueda.h"
#include <cctype>
#include <iterator>
#include <queue>

using namespace std;

MotorBusqueda::MotorBusqueda() {
    raicesST.push_back(new NodoST());
}

void MotorBusqueda::liberarST(NodoST* nodo) {
    if (!nodo) return;
    for (NodoST* hijo : nodo->hijos) {
        liberarST(hijo);
    }
    delete nodo;
}

MotorBusqueda::~MotorBusqueda() {
    for (NodoST* raiz : raicesST) liberarST(raiz);
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
        insertarSufijo(raicesST.front(), titLimpio.substr(i), id);
    }
}

// ============================================
// BÚSQUEDA EN SUFFIX TREE (Subcadenas)
// ============================================

vector<int> MotorBusqueda::buscarPorTitulo(const string& subcadena) {
    string query = normalizarToken(subcadena);
    if (query.empty()) return {};

    vector<int> resultados;
    for (NodoST* raiz : raicesST) {
        vector<int> parciales = buscarEnRaiz(raiz, query);
        resultados.insert(resultados.end(), parciales.begin(), parciales.end());
    }
    sort(resultados.begin(), resultados.end());
    resultados.erase(unique(resultados.begin(), resultados.end()), resultados.end());
    return resultados;
}

vector<int> MotorBusqueda::buscarEnRaiz(NodoST* raiz, const string& query) {
    NodoST* actual = raiz;
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
            // FIX: Copiar y ordenar los IDs porque la inserción concurrente rompe el orden.
            // Esto evita que algoritmos posteriores (como set_union) eliminen la mitad de los datos.
            vector<int> resultados = hijoCoincidente->peliculasIDs;
            sort(resultados.begin(), resultados.end());
            resultados.erase(unique(resultados.begin(), resultados.end()), resultados.end());

            return resultados;
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

void MotorBusqueda::prepararCarga(size_t cantidad, size_t cantidadArboles) {
    baseDatos.resize(cantidad);
    for (NodoST* raiz : raicesST) liberarST(raiz);
    raicesST.clear();
    cantidadArboles = max<size_t>(1, cantidadArboles);
    raicesST.reserve(cantidadArboles);
    for (size_t i = 0; i < cantidadArboles; ++i)
        raicesST.push_back(new NodoST());
}

void MotorBusqueda::agregarPeliculaConcurrente(const Pelicula& p, int id,
                                                size_t indiceArbol,
                                                vector<ParPalabraId>& bufferLocal) {
    if (id < 0 || static_cast<size_t>(id) >= baseDatos.size() ||
        indiceArbol >= raicesST.size()) return;

    Pelicula estable = p;
    estable.id = id;
    baseDatos[id] = std::move(estable);

    string tituloLimpio = normalizarToken(p.titulo);
    tituloLimpio += "$";
    NodoST* raizLocal = raicesST[indiceArbol];
    for (size_t i = 0; i < tituloLimpio.length(); ++i)
        insertarSufijo(raizLocal, tituloLimpio.substr(i), id);

    tokenizarYAgregar(p.trama, id, bufferLocal);
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

void MotorBusqueda::mergeBuffers(vector<vector<ParPalabraId>>& buffersLocales) {
    size_t total = 0;
    for (const auto& b : buffersLocales) total += b.size();
    bufferIndexacion.reserve(bufferIndexacion.size() + total);
    for (auto& b : buffersLocales) {
        bufferIndexacion.insert(bufferIndexacion.end(),
                                make_move_iterator(b.begin()),
                                make_move_iterator(b.end()));
    }
}

void MotorBusqueda::finalizarIndexacionParalela(
    vector<vector<ParPalabraId>>& buffersLocales) {
    struct Cursor {
        size_t buffer;
        size_t posicion;
    };
    struct MayorPrimero {
        const vector<vector<ParPalabraId>>* buffers;
        bool operator()(const Cursor& a, const Cursor& b) const {
            return (*buffers)[b.buffer][b.posicion] <
                   (*buffers)[a.buffer][a.posicion];
        }
    };

    priority_queue<Cursor, vector<Cursor>, MayorPrimero>
        cola(MayorPrimero{&buffersLocales});

    size_t totalPalabras = 0;
    for (size_t i = 0; i < buffersLocales.size(); ++i) {
        totalPalabras += buffersLocales[i].size();
        if (!buffersLocales[i].empty()) cola.push({i, 0});
    }

    indiceInvertido.clear();
    indiceInvertido.reserve(totalPalabras / 4);

    while (!cola.empty()) {
        Cursor cursor = cola.top();
        cola.pop();
        const ParPalabraId& par =
            buffersLocales[cursor.buffer][cursor.posicion];

        if (indiceInvertido.empty() ||
            indiceInvertido.back().palabra != par.palabra) {
            indiceInvertido.push_back({par.palabra, {par.id}});
        } else if (indiceInvertido.back().ids.back() != par.id) {
            indiceInvertido.back().ids.push_back(par.id);
        }

        ++cursor.posicion;
        if (cursor.posicion < buffersLocales[cursor.buffer].size())
            cola.push(cursor);
    }

    for (auto& buffer : buffersLocales) {
        buffer.clear();
        buffer.shrink_to_fit();
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
    if (termino.empty()) return {};

    // 1. Tokenizar la búsqueda de la misma manera que se tokenizó la trama
    vector<string> palabrasBusqueda;
    string palabra;
    for (size_t i = 0; i <= termino.length(); ++i) {
        if (i == termino.length() || isspace((unsigned char)termino[i])) {
            if (!palabra.empty()) {
                string limpia = normalizarToken(palabra);
                if (limpia.length() > 2) palabrasBusqueda.push_back(limpia);
                palabra.clear();
            }
        } else {
            palabra += termino[i];
        }
    }

    if (palabrasBusqueda.empty()) return {};

    vector<int> resultadosFinales;
    bool primeraPalabra = true;

    // 2. Buscar cada palabra y unificar los resultados
    for (const string& busqueda : palabrasBusqueda) {
        vector<int> resultadosParciales;

        auto it = lower_bound(indiceInvertido.begin(), indiceInvertido.end(), busqueda,
            [](const EntradaIndice& e, const string& val) {
                return e.palabra < val;
            });

        if (it != indiceInvertido.end() && it->palabra == busqueda)
            resultadosParciales = it->ids;

        for (const auto& entrada : indiceInvertido) {
            if (entrada.palabra != busqueda &&
                entrada.palabra.find(busqueda) != string::npos) {
                resultadosParciales.insert(resultadosParciales.end(), entrada.ids.begin(), entrada.ids.end());
            }
        }

        sort(resultadosParciales.begin(), resultadosParciales.end());
        resultadosParciales.erase(unique(resultadosParciales.begin(), resultadosParciales.end()), resultadosParciales.end());

        // Si es la primera palabra, inicializamos. Si no, podemos hacer una unión (o intersección, según prefieras)
        if (primeraPalabra) {
            resultadosFinales = move(resultadosParciales);
            primeraPalabra = false;
        } else {
            vector<int> combinados;
            // Usamos set_union para que devuelva la pelicula si coincide al menos una palabra
            // Usa set_intersection si quieres ser estricto (ambas palabras deben estar)
            set_union(resultadosFinales.begin(), resultadosFinales.end(),
                      resultadosParciales.begin(), resultadosParciales.end(),
                      back_inserter(combinados));
            resultadosFinales = move(combinados);
        }
    }

    return resultadosFinales;
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
