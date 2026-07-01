#ifndef INTERFAZ_STREAMING_H
#define INTERFAZ_STREAMING_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <memory>
#include "MotorBusqueda.h"
#include "EstadoInterfaz.h"   // <-- Patrón State

#include "PerfilMemento.h"
#include "ObservadorPerfil.h"
#include "EstrategiaRelevancia.h"

using namespace std;

// ============================================================
// ESTRUCTURAS
// ============================================================

struct PerfilUsuario {
    vector<int> likes;
    vector<int> verMasTarde;
    map<string, int> frecuenciaGeneros;
};

struct ResultadoBusqueda {
    int id;
    double relevancia;
    string tipoCoincidencia;
};

template <typename Contenedor, typename Valor>
bool contiene(const Contenedor& contenedor, const Valor& valor) {
    return find(contenedor.begin(), contenedor.end(), valor) != contenedor.end();
}


// CLASE CONTEXTO - InterfazStreaming
//
// En el patrón State esta clase actúa como Contexto (Context).
// Mantiene una referencia al estado activo (estadoActual) y
// delega la lógica de cada pantalla a dicho estado.
//
// Los métodos de pantalla (pantallaBusqueda, etc.) pasan a ser
// públicos para que los estados concretos puedan invocarlos,
// dado que son estados separados y no subclases.


class InterfazStreaming {
private:
    MotorBusqueda*   motor;
    PerfilUsuario    perfil;
    EstadoInterfaz*  estadoPendiente;
    CuidadorPerfil cuidadorPerfil;
    vector<unique_ptr<ObservadorPerfil>> observadores;
    unique_ptr<EstrategiaRelevancia> estrategia;
    EstadoInterfaz*  estadoActual;   // Estado activo (patrón State)

    // ---- Utilidades internas ----
    vector<string> tokenizar(const string& consulta);
    bool           tieneLike(int id);
    bool           tieneVerMasTarde(int id);
    void           eliminarDeVector(vector<int>& vec, int id);
    double         calcularRelevancia(int id, const vector<string>& terminos,
                                      bool enTitulo, bool enTrama, bool enTag = false);
    vector<ResultadoBusqueda> buscar(const string& consulta);
    void           toggleLike(int id);
    void           toggleVerMasTarde(int id);
    PerfilMemento  crearMemento() const;
    void           restaurarMemento(const PerfilMemento& memento);
    void           notificar(EventoPerfil evento, const string& titulo);
    vector<ResultadoBusqueda> recomendaciones(int cantidad);
    void           mostrarResumen(int id, int numero, double relevancia);
    void           paginarResultados(const vector<ResultadoBusqueda>& resultados,
                                     const string& tituloBusqueda);

public:
    // Constructor / Destructor
    explicit InterfazStreaming(MotorBusqueda* m);
    ~InterfazStreaming();

    void cambiarEstado(EstadoInterfaz* nuevoEstado);


    void irAlMenu();

    // Bucle principal: delega en estadoActual->ejecutar() cada iteracion.
    void ejecutar();

    // ---- Métodos de pantalla (públicos para los estados) ----
    void pantallaInicio();
    void pantallaBusqueda();
    void pantallaVerMasTarde();
    void pantallaMisLikes();
    void pantallaEstadisticas();
    void mostrarDetalle(int id);

    // ---- Utilidades visuales (públicas para los estados) ----
    void limpiarPantalla();
    void pausar();
    void linea(int ancho, char c);
    void titulo(const string& texto);
};

#endif // INTERFAZ_STREAMING_H
