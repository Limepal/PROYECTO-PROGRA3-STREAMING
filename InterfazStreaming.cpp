#include "InterfazStreaming.h"
#include <vector>
#include <algorithm>

using namespace std;

// ============================================================
// CONSTRUCTOR
// ============================================================

InterfazStreaming::InterfazStreaming(MotorBusqueda* m) : motor(m) {}

// ============================================================
// UTILIDADES VISUALES
// ============================================================

void InterfazStreaming::limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void InterfazStreaming::pausar() {
    cout << "Presiona ENTER para continuar...";
    cin.ignore();
    cin.get();
}

void InterfazStreaming::linea(int ancho, char c) {
    cout << string(ancho, c) << endl;
}

void InterfazStreaming::titulo(const string& texto) {
    limpiarPantalla();
    linea(60, '=');
    int espacios = (60 - (int)texto.length()) / 2;
    if (espacios < 0) espacios = 0;
    cout << string(espacios, ' ') << texto << endl;
    linea(60, '=');
}

// ============================================================
// TOKENIZACION
// ============================================================

vector<string> InterfazStreaming::tokenizar(const string& consulta) {
    vector<string> tokens;
    string token;
    for (char c : consulta) {
        if (isalnum((unsigned char)c)) {
            token += (char)tolower((unsigned char)c);
        } else if (!token.empty()) {
            if (token.length() > 2) tokens.push_back(token);
            token.clear();
        }
    }
    if (!token.empty() && token.length() > 2) tokens.push_back(token);
    return tokens;
}

// ============================================================
// AYUDAS PARA VECTOR (reemplazan set)
// ============================================================

bool InterfazStreaming::tieneLike(int id) {
    for (int i : perfil.likes) {
        if (i == id) return true;
    }
    return false;
}

bool InterfazStreaming::tieneVerMasTarde(int id) {
    for (int i : perfil.verMasTarde) {
        if (i == id) return true;
    }
    return false;
}

void InterfazStreaming::eliminarDeVector(vector<int>& vec, int id) {
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i] == id) {
            vec.erase(vec.begin() + i);
            return;
        }
    }
}

// ============================================================
// ALGORITMO DE RELEVANCIA
// ============================================================

double InterfazStreaming::calcularRelevancia(int id, const vector<string>& terminos,
                                               bool enTitulo, bool enTrama) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) return 0.0;

    double score = 0.0;

    if (enTitulo) {
        score += 3.0;
        for (const string& t : terminos) {
            if (p.titulo.find(t) == 0) score += 1.0;
        }
    }

    if (enTrama) score += 2.0;

    try {
        int year = stoi(p.year);
        if (year >= 2015) score += 0.5;
        else if (year >= 2010) score += 0.4;
        else if (year >= 2000) score += 0.2;
        else if (year >= 1990) score += 0.1;
    } catch (...) {}

    if (tieneVerMasTarde(id)) score += 0.2;
    if (perfil.frecuenciaGeneros.count(p.genero)) {
        score += 0.1 * perfil.frecuenciaGeneros[p.genero];
    }

    return score;
}

// ============================================================
// BUSQUEDA GENERAL
// ============================================================

vector<ResultadoBusqueda> InterfazStreaming::buscar(const string& consulta) {
    vector<string> terminos = tokenizar(consulta);
    if (terminos.empty() && consulta.length() >= 2) {
        terminos.push_back(consulta);
    }

    map<int, vector<string>> coincidencias;

    for (const string& term : terminos) {
        vector<int> idsTitulo = motor->buscarPorTitulo(term);
        for (int id : idsTitulo) {
            bool yaEsta = false;
            for (const string& s : coincidencias[id]) {
                if (s == "titulo") { yaEsta = true; break; }
            }
            if (!yaEsta) coincidencias[id].push_back("titulo");
        }

        vector<int> idsTrama = motor->buscarEnTrama(term);
        for (int id : idsTrama) {
            bool yaEsta = false;
            for (const string& s : coincidencias[id]) {
                if (s == "trama") { yaEsta = true; break; }
            }
            if (!yaEsta) coincidencias[id].push_back("trama");
        }
    }

    vector<ResultadoBusqueda> resultados;
    for (const auto& par : coincidencias) {
        ResultadoBusqueda rb;
        rb.id = par.first;
        bool enTitulo = false, enTrama = false;
        for (const string& tipo : par.second) {
            if (tipo == "titulo") enTitulo = true;
            if (tipo == "trama") enTrama = true;
        }
        rb.relevancia = calcularRelevancia(par.first, terminos, enTitulo, enTrama);
        if (enTitulo) rb.tipoCoincidencia = "titulo";
        else rb.tipoCoincidencia = "trama";
        resultados.push_back(rb);
    }

    sort(resultados.begin(), resultados.end(),
         [](const ResultadoBusqueda& a, const ResultadoBusqueda& b) {
             return a.relevancia > b.relevancia;
         });

    return resultados;
}

// ============================================================
// GESTION DE LIKES Y VER MAS TARDE (con vector)
// ============================================================

void InterfazStreaming::toggleLike(int id) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) return;

    if (tieneLike(id)) {
        eliminarDeVector(perfil.likes, id);
        perfil.frecuenciaGeneros[p.genero]--;
        if (perfil.frecuenciaGeneros[p.genero] <= 0)
            perfil.frecuenciaGeneros.erase(p.genero);
        cout << "Like removido: " << p.titulo << endl;
    } else {
        perfil.likes.push_back(id);
        perfil.frecuenciaGeneros[p.genero]++;
        cout << "Like agregado: " << p.titulo << endl;
    }
}

void InterfazStreaming::toggleVerMasTarde(int id) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) return;

    if (tieneVerMasTarde(id)) {
        eliminarDeVector(perfil.verMasTarde, id);
        cout << "Removido de Ver mas tarde: " << p.titulo << endl;
    } else {
        perfil.verMasTarde.push_back(id);
        cout << "Agregado a Ver mas tarde: " << p.titulo << endl;
    }
}

// ============================================================
// ALGORITMO DE RECOMENDACION
// ============================================================

vector<ResultadoBusqueda> InterfazStreaming::recomendaciones(int cantidad) {
    vector<ResultadoBusqueda> recs;

    if (perfil.likes.empty()) {
        return recs;
    }

    map<int, double> scores;

    for (int likedId : perfil.likes) {
        Pelicula liked = motor->obtenerPelicula(likedId);
        if (liked.id == -1) continue;

        vector<string> palabras = tokenizar(liked.trama);
        for (const string& palabra : palabras) {
            if (palabra.length() > 4) {
                vector<int> similares = motor->buscarEnTrama(palabra);
                for (int id : similares) {
                    if (id != likedId && !tieneLike(id)) {
                        Pelicula cand = motor->obtenerPelicula(id);
                        double bonus = (cand.genero == liked.genero) ? 1.0 : 0.0;
                        scores[id] += 0.3 + bonus;
                    }
                }
            }
        }
    }

    for (const auto& par : scores) {
        ResultadoBusqueda rb;
        rb.id = par.first;
        rb.relevancia = par.second;
        rb.tipoCoincidencia = "recomendacion";
        recs.push_back(rb);
    }

    sort(recs.begin(), recs.end(),
         [](const ResultadoBusqueda& a, const ResultadoBusqueda& b) {
             return a.relevancia > b.relevancia;
         });

    if (recs.size() > (size_t)cantidad) recs.resize(cantidad);
    return recs;
}

// ============================================================
// VISUALIZACION
// ============================================================

void InterfazStreaming::mostrarResumen(int id, int numero, double relevancia) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) return;

    bool vmt = tieneVerMasTarde(id);
    bool like = tieneLike(id);

    cout << " [" << numero << "] ";
    cout << p.titulo;
    cout << " (" << p.year << ")";
    cout << " [" << p.genero << "]";

    if (relevancia >= 0) {
        cout << " - Relevancia: " << fixed << setprecision(1) << relevancia;
    }

    if (like) cout << " [LIKE]";
    if (vmt) cout << " [VMT]";

    cout << endl;
}

void InterfazStreaming::mostrarDetalle(int id) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) {
        cout << "Pelicula no encontrada." << endl;
        return;
    }

    titulo("DETALLE DE PELICULA");

    cout << endl;
    cout << "TITULO: " << p.titulo << endl;
    linea(60, '-');
    cout << "Ano:      " << p.year << endl;
    cout << "Genero:   " << p.genero << endl;
    cout << "Director: " << p.director << endl;
    cout << "Reparto:  " << p.reparto << endl;
    cout << "Origen:   " << p.origen << endl;
    linea(60, '-');
    cout << "SINOPSIS:" << endl;

    string trama = p.trama;
    int ancho = 55;
    size_t pos = 0;
    while (pos < trama.length()) {
        size_t corte = min(pos + (size_t)ancho, trama.length());
        if (corte < trama.length()) {
            size_t esp = trama.rfind(' ', corte);
            if (esp != string::npos && esp > pos) corte = esp;
        }
        cout << "  " << trama.substr(pos, corte - pos) << endl;
        pos = corte;
        while (pos < trama.length() && trama[pos] == ' ') pos++;
    }

    linea(60, '-');

    bool like = tieneLike(id);
    bool vmt = tieneVerMasTarde(id);

    cout << endl;
    cout << "OPCIONES:" << endl;
    cout << "  [1] " << (like ? "Quitar Like" : "Dar Like") << endl;
    cout << "  [2] " << (vmt ? "Quitar de Ver mas tarde" : "Ver mas tarde") << endl;
    cout << "  [3] Volver" << endl;
    cout << "  [0] Menu principal" << endl;

    int op;
    cout << "Selecciona: ";
    cin >> op;

    switch (op) {
        case 1: toggleLike(id); break;
        case 2: toggleVerMasTarde(id); break;
        case 0: return;
        default: break;
    }
}

void InterfazStreaming::paginarResultados(const vector<ResultadoBusqueda>& resultados,
                                            const string& tituloBusqueda) {
    if (resultados.empty()) {
        titulo("RESULTADOS");
        cout << "No se encontraron peliculas para: "" << tituloBusqueda << """ << endl;
        pausar();
        return;
    }

    size_t pagina = 0;
    const size_t POR_PAGINA = 5;

    while (true) {
        titulo("RESULTADOS: "" + tituloBusqueda + """);

        size_t inicio = pagina * POR_PAGINA;
        size_t fin = min(inicio + POR_PAGINA, resultados.size());

        cout << "Mostrando " << (fin - inicio) << " de " << resultados.size()
             << " resultados (pagina " << (pagina + 1) << " de "
             << ((resultados.size() + POR_PAGINA - 1) / POR_PAGINA) << ")" << endl;
        cout << endl;

        for (size_t i = inicio; i < fin; i++) {
            mostrarResumen(resultados[i].id, (int)(i + 1), resultados[i].relevancia);
        }

        linea(60, '-');
        cout << endl;
        cout << "OPCIONES:" << endl;
        if (fin < resultados.size())
            cout << "  [+] Ver siguientes 5" << endl;
        if (pagina > 0)
            cout << "  [-] Ver anteriores 5" << endl;
        cout << "  [1-" << (fin - inicio) << "] Ver detalle" << endl;
        cout << "  [0] Volver al menu" << endl;

        string op;
        cout << "Selecciona: ";
        cin >> op;

        if (op == "+" && fin < resultados.size()) {
            pagina++;
        } else if (op == "-" && pagina > 0) {
            pagina--;
        } else if (op == "0") {
            return;
        } else {
            try {
                int num = stoi(op);
                if (num >= 1 && num <= (int)(fin - inicio)) {
                    mostrarDetalle(resultados[inicio + num - 1].id);
                }
            } catch (...) {
                cout << "Opcion invalida." << endl;
                pausar();
            }
        }
    }
}

// ============================================================
// PANTALLAS
// ============================================================

void InterfazStreaming::pantallaInicio() {
    titulo("STREAMFLIX - INICIO");

    if (!perfil.verMasTarde.empty()) {
        cout << "TU LISTA 'VER MAS TARDE':" << endl;
        linea(60, '-');

        size_t limite = min(perfil.verMasTarde.size(), size_t(5));
        for (size_t i = 0; i < limite; i++) {
            mostrarResumen(perfil.verMasTarde[i], (int)(i + 1));
        }

        if (perfil.verMasTarde.size() > 5) {
            cout << "... y " << (perfil.verMasTarde.size() - 5) << " mas" << endl;
        }
        cout << endl;
    }

    cout << "RECOMENDACIONES PARA TI:" << endl;
    linea(60, '-');

    vector<ResultadoBusqueda> recs = recomendaciones(5);
    if (recs.empty()) {
        cout << "Aun no hay recomendaciones personalizadas." << endl;
        cout << "Da 'Like' a peliculas para obtener sugerencias." << endl;
    } else {
        for (size_t i = 0; i < recs.size(); i++) {
            mostrarResumen(recs[i].id, (int)(i + 1), recs[i].relevancia);
        }
    }

    linea(60, '=');
    pausar();
}

void InterfazStreaming::pantallaBusqueda() {
    titulo("BUSCAR PELICULAS");

    cout << "Puedes buscar por:" << endl;
    cout << "  - Palabra o frase en titulo o sinopsis" << endl;
    cout << "  - Sub-palabra (ej: 'bar' encuentra 'barco')" << endl;
    cout << endl;
    cout << "NOTA: Busqueda por director/reparto/genero requiere" << endl;
    cout << "      extension del Motor de Busqueda." << endl;
    cout << endl;

    cout << "Ingresa tu busqueda: ";
    string consulta;
    cin.ignore();
    getline(cin, consulta);

    if (consulta.empty()) {
        cout << "Busqueda cancelada." << endl;
        pausar();
        return;
    }

    cout << "Buscando..." << endl;
    vector<ResultadoBusqueda> resultados = buscar(consulta);
    paginarResultados(resultados, consulta);
}

void InterfazStreaming::pantallaVerMasTarde() {
    titulo("VER MAS TARDE");

    if (perfil.verMasTarde.empty()) {
        cout << "No tienes peliculas guardadas." << endl;
        pausar();
        return;
    }

    vector<ResultadoBusqueda> resultados;
    for (int id : perfil.verMasTarde) {
        ResultadoBusqueda rb;
        rb.id = id;
        rb.relevancia = 0;
        rb.tipoCoincidencia = "vmt";
        resultados.push_back(rb);
    }

    paginarResultados(resultados, "Ver mas tarde");
}

void InterfazStreaming::pantallaMisLikes() {
    titulo("MIS LIKES");

    if (perfil.likes.empty()) {
        cout << "No has dado Like a ninguna pelicula." << endl;
        pausar();
        return;
    }

    vector<ResultadoBusqueda> resultados;
    for (int id : perfil.likes) {
        ResultadoBusqueda rb;
        rb.id = id;
        rb.relevancia = 0;
        rb.tipoCoincidencia = "like";
        resultados.push_back(rb);
    }

    paginarResultados(resultados, "Mis Likes");
}

void InterfazStreaming::pantallaEstadisticas() {
    titulo("TUS ESTADISTICAS");

    cout << "Peliculas con Like: " << perfil.likes.size() << endl;
    cout << "Ver mas tarde:      " << perfil.verMasTarde.size() << endl;
    cout << "Total en base:      " << motor->getTotalPeliculas() << endl;

    if (!perfil.frecuenciaGeneros.empty()) {
        cout << endl;
        cout << "Tus generos favoritos:" << endl;
        vector<pair<string, int>> generos(perfil.frecuenciaGeneros.begin(),
                                           perfil.frecuenciaGeneros.end());
        sort(generos.begin(), generos.end(),
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });
        for (const auto& g : generos) {
            cout << "  " << g.first << ": " << g.second << " likes" << endl;
        }
    }

    pausar();
}

// ============================================================
// MENU PRINCIPAL
// ============================================================

void InterfazStreaming::ejecutar() {
    pantallaInicio();

    while (true) {
        titulo("STREAMFLIX - MENU PRINCIPAL");

        cout << "  [1] Buscar peliculas" << endl;
        cout << "  [2] Ver mas tarde" << endl;
        cout << "  [3] Mis Likes" << endl;
        cout << "  [4] Estadisticas" << endl;
        cout << "  [5] Pantalla de inicio" << endl;
        cout << "  [0] Salir" << endl;

        linea(60, '=');
        cout << "Selecciona: ";

        int op;
        cin >> op;

        switch (op) {
            case 1: pantallaBusqueda(); break;
            case 2: pantallaVerMasTarde(); break;
            case 3: pantallaMisLikes(); break;
            case 4: pantallaEstadisticas(); break;
            case 5: pantallaInicio(); break;
            case 0:
                titulo("HASTA PRONTO");
                cout << "Gracias por usar StreamFlix." << endl;
                return;
            default:
                cout << "Opcion invalida." << endl;
                pausar();
        }
    }
}