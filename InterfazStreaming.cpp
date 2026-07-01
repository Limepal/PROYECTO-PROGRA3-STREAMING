#include "InterfazStreaming.h"
#include "EstadosConcretos.h"
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

// CONSTRUCTOR / DESTRUCTOR

InterfazStreaming::InterfazStreaming(MotorBusqueda* m)
    : motor(m),
      estadoPendiente(nullptr),
      cuidadorPerfil("perfil_usuario.csv"),
      estrategia(make_unique<RelevanciaPonderada>()),
      estadoActual(nullptr)
{
    observadores.push_back(make_unique<NotificadorConsola>());
    if (cuidadorPerfil.existe())
        restaurarMemento(cuidadorPerfil.restaurar());
    perfil.likes.erase(remove_if(perfil.likes.begin(), perfil.likes.end(),
        [this](int id) { return motor->obtenerPelicula(id).id == -1; }), perfil.likes.end());
    perfil.verMasTarde.erase(remove_if(perfil.verMasTarde.begin(), perfil.verMasTarde.end(),
        [this](int id) { return motor->obtenerPelicula(id).id == -1; }), perfil.verMasTarde.end());
    for (int id : perfil.likes)
        perfil.frecuenciaGeneros[motor->obtenerPelicula(id).genero]++;
    // Estado inicial: pantalla de inicio (ver más tarde + recomendaciones)
    estadoActual = new EstadoInicio();
}

InterfazStreaming::~InterfazStreaming() {
    cuidadorPerfil.guardar(crearMemento());
    delete estadoActual;
    delete estadoPendiente;
}


// PATRON STATE - cambiarEstado
//
// Libera el estado anterior y activa el nuevo.
// Los estados concretos llaman a este método para hacer
// transiciones sin que el contexto conozca los detalles.

void InterfazStreaming::cambiarEstado(EstadoInterfaz* nuevoEstado) {
    delete estadoPendiente;
    estadoPendiente = nuevoEstado;
}

// irAlMenu() se implementa AQUI (no en el .h) porque en este
// punto EstadosConcretos.h ya fue incluido, por lo que EstadoMenu
// es un tipo completo y "new EstadoMenu()" compila sin problemas.
void InterfazStreaming::irAlMenu() {
    cambiarEstado(new EstadoMenu());

}


// BUCLE PRINCIPAL - delega en el estado activo
//
// Antes: switch(op) { case 1: pantallaBusqueda(); ... }
// Ahora: el estado activo decide qué hacer y a dónde ir.

void InterfazStreaming::ejecutar() {
    while (estadoActual != nullptr) {
        bool continuar = estadoActual->ejecutar(*this);
        if (!continuar) break;
        if (estadoPendiente != nullptr) {
            delete estadoActual;
            estadoActual = estadoPendiente;
            estadoPendiente = nullptr;
        }
    }
}


// UTILIDADES VISUALES

void InterfazStreaming::limpiarPantalla() {
    // La ventana Run de CLion no siempre emula una terminal: `cls` puede
    // recortar la salida y las secuencias ANSI pueden mostrarse como texto.
    // Separar las vistas es portable y garantiza que el menu se vea completo.
    cout << "\n\n" << flush;
}

void InterfazStreaming::pausar() {
    cout << "Presiona ENTER para continuar...";
    string espera;
    getline(cin, espera);
}

void InterfazStreaming::linea(int ancho, char c) {
    cout << string(ancho, c) << endl;
}

void InterfazStreaming::titulo(const string& texto) {
    limpiarPantalla();
    int espacios = (60 - (int)texto.length()) / 2;
    if (espacios < 0) espacios = 0;
    const string borde(60, '=');
    cout << borde << '\n'
         << string(espacios, ' ') << texto << '\n'
         << borde << '\n'
         << flush;
}


// TOKENIZACION

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


// AYUDAS PARA VECTOR

bool InterfazStreaming::tieneLike(int id) {
    return contiene(perfil.likes, id);
}

bool InterfazStreaming::tieneVerMasTarde(int id) {
    return contiene(perfil.verMasTarde, id);
}

void InterfazStreaming::eliminarDeVector(vector<int>& vec, int id) {
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i] == id) {
            vec.erase(vec.begin() + i);
            return;
        }
    }
}


// ALGORITMO DE RELEVANCIA

double InterfazStreaming::calcularRelevancia(int id,
                                               const vector<string>& terminos,
                                               bool enTitulo, bool enTrama, bool enTag) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) return 0.0;
    return estrategia->calcular(p, terminos, enTitulo, enTrama, enTag,
                                tieneVerMasTarde(id), perfil.frecuenciaGeneros);
}

PerfilMemento InterfazStreaming::crearMemento() const {
    return {perfil.likes, perfil.verMasTarde};
}

void InterfazStreaming::restaurarMemento(const PerfilMemento& memento) {
    perfil.likes = memento.obtenerLikes();
    perfil.verMasTarde = memento.obtenerVerMasTarde();
}

void InterfazStreaming::notificar(EventoPerfil evento, const string& nombre) {
    for (const auto& observador : observadores)
        observador->actualizar(evento, nombre);
}


// BUSQUEDA GENERAL

vector<ResultadoBusqueda> InterfazStreaming::buscar(const string& consulta) {
    string campoTag, valorConsulta = consulta;
    const size_t separador = consulta.find(':');
    if (separador != string::npos) {
        campoTag = motor->normalizarToken(consulta.substr(0, separador));
        valorConsulta = consulta.substr(separador + 1);
    }
    vector<string> terminos = tokenizar(valorConsulta);
    if (terminos.empty() && valorConsulta.length() >= 2)
        terminos.push_back(valorConsulta);

    map<int, vector<string>> coincidencias;

    if (!campoTag.empty()) {
        for (int id : motor->buscarPorTag(campoTag, valorConsulta))
            coincidencias[id].push_back("tag");
    }

    for (const string& term : terminos) {
        if (!campoTag.empty()) break;
        for (int id : motor->buscarPorTitulo(term)) {
            bool yaEsta = false;
            for (const string& s : coincidencias[id])
                if (s == "titulo") { yaEsta = true; break; }
            if (!yaEsta) coincidencias[id].push_back("titulo");
        }
        for (int id : motor->buscarEnTrama(term)) {
            bool yaEsta = false;
            for (const string& s : coincidencias[id])
                if (s == "trama") { yaEsta = true; break; }
            if (!yaEsta) coincidencias[id].push_back("trama");
        }
    }

    vector<ResultadoBusqueda> resultados;
    for (const auto& par : coincidencias) {
        ResultadoBusqueda rb;
        rb.id = par.first;
        bool enTitulo = false, enTrama = false, enTag = false;
        for (const string& tipo : par.second) {
            if (tipo == "titulo") enTitulo = true;
            if (tipo == "trama")  enTrama  = true;
            if (tipo == "tag")    enTag    = true;
        }
        rb.relevancia        = calcularRelevancia(par.first, terminos, enTitulo, enTrama, enTag);
        rb.tipoCoincidencia  = enTitulo ? "titulo" : enTrama ? "trama" : "tag";
        resultados.push_back(rb);
    }

    sort(resultados.begin(), resultados.end(),
         [](const ResultadoBusqueda& a, const ResultadoBusqueda& b) {
             return a.relevancia > b.relevancia;
         });

    return resultados;
}


// GESTION DE LIKES Y VER MAS TARDE


void InterfazStreaming::toggleLike(int id) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) return;

    if (tieneLike(id)) {
        eliminarDeVector(perfil.likes, id);
        perfil.frecuenciaGeneros[p.genero]--;
        if (perfil.frecuenciaGeneros[p.genero] <= 0)
            perfil.frecuenciaGeneros.erase(p.genero);
        notificar(EventoPerfil::LikeEliminado, p.titulo);
    } else {
        perfil.likes.push_back(id);
        perfil.frecuenciaGeneros[p.genero]++;
        notificar(EventoPerfil::LikeAgregado, p.titulo);
    }
    if (!cuidadorPerfil.guardar(crearMemento()))
        cerr << "Advertencia: no se pudo guardar el perfil." << endl;
}

void InterfazStreaming::toggleVerMasTarde(int id) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) return;

    if (tieneVerMasTarde(id)) {
        eliminarDeVector(perfil.verMasTarde, id);
        notificar(EventoPerfil::PendienteEliminado, p.titulo);
    } else {
        perfil.verMasTarde.push_back(id);
        notificar(EventoPerfil::PendienteAgregado, p.titulo);
    }
    if (!cuidadorPerfil.guardar(crearMemento()))
        cerr << "Advertencia: no se pudo guardar el perfil." << endl;
}


// ALGORITMO DE RECOMENDACION

vector<ResultadoBusqueda> InterfazStreaming::recomendaciones(int cantidad) {
    vector<ResultadoBusqueda> recs;
    if (perfil.likes.empty()) return recs;

    map<int, double> scores;

    for (int likedId : perfil.likes) {
        Pelicula liked = motor->obtenerPelicula(likedId);
        if (liked.id == -1) continue;

        for (const string& palabra : tokenizar(liked.trama)) {
            if (palabra.length() > 4) {
                for (int id : motor->buscarEnTrama(palabra)) {
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
        rb.id               = par.first;
        rb.relevancia       = par.second;
        rb.tipoCoincidencia = "recomendacion";
        recs.push_back(rb);
    }

    sort(recs.begin(), recs.end(),
         [](const ResultadoBusqueda& a, const ResultadoBusqueda& b) {
             return a.relevancia > b.relevancia;
         });

    if ((int)recs.size() > cantidad)
        recs.resize(cantidad);

    return recs;
}


// VISUALIZACION

void InterfazStreaming::mostrarResumen(int id, int numero, double relevancia) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) return;

    string like = tieneLike(id)        ? "[♥]" : "   ";
    string vmt  = tieneVerMasTarde(id) ? "[⏱]" : "   ";

    cout << numero << ". " << like << vmt << " "
         << p.titulo << " (" << p.year << ")" << endl;
    cout << "   Genero: " << p.genero
         << " | Relevancia: " << fixed << setprecision(1) << relevancia << endl;
    linea(60, '-');
}

void InterfazStreaming::mostrarDetalle(int id) {
    Pelicula p = motor->obtenerPelicula(id);
    if (p.id == -1) {
        cout << "Pelicula no encontrada." << endl;
        pausar();
        return;
    }

    titulo(p.titulo);

    cout << "Año:      " << p.year     << endl;
    cout << "Genero:   " << p.genero   << endl;
    cout << "Director: " << p.director << endl;
    cout << "Reparto:  " << p.reparto  << endl;
    cout << "Origen:   " << p.origen   << endl;
    linea(60, '-');
    cout << "SINOPSIS:" << endl;

    // Imprimir trama en líneas de ~55 chars
    const int ANCHO = 55;
    string trama = p.trama;
    size_t pos = 0;
    while (pos < trama.length()) {
        size_t fin = min(pos + (size_t)ANCHO, trama.length());
        if (fin < trama.length()) {
            size_t espacio = trama.rfind(' ', fin);
            if (espacio != string::npos && espacio > pos) fin = espacio;
        }
        cout << "  " << trama.substr(pos, fin - pos) << endl;
        pos = fin;
        while (pos < trama.length() && trama[pos] == ' ') pos++;
    }

    linea(60, '-');

    bool like = tieneLike(id);
    bool vmt  = tieneVerMasTarde(id);

    cout << endl;
    cout << "OPCIONES:" << endl;
    cout << "  [1] " << (like ? "Quitar Like"               : "Dar Like")       << endl;
    cout << "  [2] " << (vmt  ? "Quitar de Ver mas tarde"   : "Ver mas tarde")  << endl;
    cout << "  [3] Volver" << endl;
    cout << "  [0] Menu principal" << endl;

    int op;
    cout << "Selecciona: ";
    cin >> op;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch (op) {
        case 1: toggleLike(id);         break;
        case 2: toggleVerMasTarde(id);  break;
        case 0: /* cambiará a Menu desde el estado */  return;
        default: break;
    }
}

void InterfazStreaming::paginarResultados(const vector<ResultadoBusqueda>& resultados,
                                          const string& tituloBusqueda) {
    if (resultados.empty()) {
        titulo("RESULTADOS");
        cout << "No se encontraron peliculas para: \"" << tituloBusqueda << "\"" << endl;
        pausar();
        return;
    }

    size_t pagina = 0;
    const size_t POR_PAGINA = 5;

    while (true) {
        titulo("RESULTADOS: \"" + tituloBusqueda + "\"");

        size_t inicio = pagina * POR_PAGINA;
        size_t fin    = min(inicio + POR_PAGINA, resultados.size());

        cout << "Mostrando " << (fin - inicio) << " de " << resultados.size()
             << " resultados (pagina " << (pagina + 1) << " de "
             << ((resultados.size() + POR_PAGINA - 1) / POR_PAGINA) << ")" << endl;
        cout << endl;

        for (size_t i = inicio; i < fin; i++)
            mostrarResumen(resultados[i].id, (int)(i + 1), resultados[i].relevancia);

        linea(60, '-');
        cout << endl;
        cout << "OPCIONES:" << endl;
        if (fin < resultados.size())
            cout << "  [+] Ver siguientes 5" << endl;
        if (pagina > 0)
            cout << "  [-] Ver anteriores 5" << endl;
        cout << "  [1-" << (fin - inicio) << "] Ver detalle" << endl;
        cout << "  [0] Volver al menu"  << endl;

        string op;
        cout << "Selecciona: ";
        cin >> op;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if      (op == "+" && fin < resultados.size()) { pagina++;   }
        else if (op == "-" && pagina > 0)              { pagina--;   }
        else if (op == "0")                            { return;      }
        else {
            try {
                int num = stoi(op);
                if (num >= 1 && num <= (int)(fin - inicio))
                    mostrarDetalle(resultados[inicio + num - 1].id);
            } catch (...) {
                cout << "Opcion invalida." << endl;
                pausar();
            }
        }
    }
}


// PANTALLAS (llamadas por los estados concretos)

void InterfazStreaming::pantallaInicio() {
    titulo("PILIFLIX - INICIO");

    if (!perfil.verMasTarde.empty()) {
        cout << "TU LISTA 'VER MAS TARDE':" << endl;
        linea(60, '-');
        size_t i = 0;
        for (int id : perfil.verMasTarde) {
            mostrarResumen(id, (int)(++i), 0.0);
            if (i == 5) break;
        }
        if (perfil.verMasTarde.size() > 5)
            cout << "... y " << (perfil.verMasTarde.size() - 5) << " mas" << endl;
        cout << endl;
    }

    cout << "RECOMENDACIONES PARA TI:" << endl;
    linea(60, '-');

    vector<ResultadoBusqueda> recs = recomendaciones(5);
    if (recs.empty()) {
        cout << "Aun no hay recomendaciones personalizadas." << endl;
        cout << "Da 'Like' a peliculas para obtener sugerencias." << endl;
    } else {
        for (size_t i = 0; i < recs.size(); i++)
            mostrarResumen(recs[i].id, (int)(i + 1), recs[i].relevancia);
    }

    linea(60, '=');
    pausar();
}

void InterfazStreaming::pantallaBusqueda() {
    titulo("BUSCAR PELICULAS");

    cout << "Puedes buscar por:" << endl;
    cout << "  - Palabra o frase en titulo o sinopsis" << endl;
    cout << "  - Sub-palabra (ej: 'bar' encuentra 'barco')" << endl;
    cout << "  - Tags: director:nolan, reparto:chaplin, genero:drama," << endl;
    cout << "          origen:american o year:2010" << endl;
    cout << endl;

    cout << "Ingresa tu busqueda: ";
    string consulta;
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
        rb.id               = id;
        rb.relevancia       = 0;
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
        rb.id               = id;
        rb.relevancia       = 0;
        rb.tipoCoincidencia = "like";
        resultados.push_back(rb);
    }

    paginarResultados(resultados, "Mis Likes");
}

void InterfazStreaming::pantallaEstadisticas() {
    titulo("TUS ESTADISTICAS");

    cout << "Peliculas con Like: " << perfil.likes.size()       << endl;
    cout << "Ver mas tarde:      " << perfil.verMasTarde.size() << endl;
    cout << "Total en base:      " << motor->getTotalPeliculas() << endl;

    if (!perfil.frecuenciaGeneros.empty()) {
        cout << endl << "Tus generos favoritos:" << endl;
        vector<pair<string,int>> generos(perfil.frecuenciaGeneros.begin(),
                                         perfil.frecuenciaGeneros.end());
        sort(generos.begin(), generos.end(),
             [](const pair<string,int>& a, const pair<string,int>& b) {
                 return a.second > b.second;
             });
        for (const auto& g : generos)
            cout << "  " << g.first << ": " << g.second << " likes" << endl;
    }

    pausar();
}
