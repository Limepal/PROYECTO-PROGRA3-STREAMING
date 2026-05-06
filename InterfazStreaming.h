#ifndef INTERFAZ_STREAMING_H
#define INTERFAZ_STREAMING_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include "MotorBusqueda.h"

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

// ============================================================
// CLASE INTERFAZ STREAMING
// ============================================================

class InterfazStreaming {
private:
    MotorBusqueda* motor;
    PerfilUsuario perfil;

    // Utilidades
    void limpiarPantalla();
    void pausar();
    void linea(int ancho, char c);
    void titulo(const string& texto);

    // Tokenizacion
    vector<string> tokenizar(const string& consulta);

    // Ayudas para vector (reemplazan set)
    bool tieneLike(int id);
    bool tieneVerMasTarde(int id);
    void eliminarDeVector(vector<int>& vec, int id);

    // Relevancia
    double calcularRelevancia(int id, const vector<string>& terminos,
                               bool enTitulo, bool enTrama);

    // Busqueda
    vector<ResultadoBusqueda> buscar(const string& consulta);

    // Gestion de perfil
    void toggleLike(int id);
    void toggleVerMasTarde(int id);

    // Recomendacion
    vector<ResultadoBusqueda> recomendaciones(int cantidad);

    // Visualizacion
    void mostrarResumen(int id, int numero, double relevancia);
    void mostrarDetalle(int id);
    void paginarResultados(const vector<ResultadoBusqueda>& resultados,
                           const string& tituloBusqueda);

    // Pantallas
    void pantallaInicio();
    void pantallaBusqueda();
    void pantallaVerMasTarde();
    void pantallaMisLikes();
    void pantallaEstadisticas();

public:
    InterfazStreaming(MotorBusqueda* m);
    void ejecutar();
};

#endif