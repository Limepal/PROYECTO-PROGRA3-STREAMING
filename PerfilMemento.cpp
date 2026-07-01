#include "PerfilMemento.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <utility>

using namespace std;

PerfilMemento::PerfilMemento(vector<int> likesGuardados,
                             vector<int> pendientesGuardados)
    : likes(std::move(likesGuardados)),
      verMasTarde(std::move(pendientesGuardados)) {}

const vector<int>& PerfilMemento::obtenerLikes() const { return likes; }
const vector<int>& PerfilMemento::obtenerVerMasTarde() const { return verMasTarde; }

CuidadorPerfil::CuidadorPerfil(string rutaArchivo) : ruta(std::move(rutaArchivo)) {}

bool CuidadorPerfil::existe() const {
    ifstream entrada(ruta);
    return entrada.good();
}

PerfilMemento CuidadorPerfil::restaurar() const {
    ifstream entrada(ruta);
    vector<int> likes, pendientes;
    string linea;
    while (getline(entrada, linea)) {
        istringstream flujo(linea);
        string tipo;
        int id = -1;
        if (getline(flujo, tipo, ',') && (flujo >> id) && id >= 0) {
            if (tipo == "LIKE") likes.push_back(id);
            else if (tipo == "VER_MAS_TARDE") pendientes.push_back(id);
        }
    }
    return {std::move(likes), std::move(pendientes)};
}

bool CuidadorPerfil::guardar(const PerfilMemento& memento) const {
    const string temporal = ruta + ".tmp";
    ofstream salida(temporal, ios::trunc);
    if (!salida.is_open()) return false;
    for (int id : memento.obtenerLikes()) salida << "LIKE," << id << '\n';
    for (int id : memento.obtenerVerMasTarde()) salida << "VER_MAS_TARDE," << id << '\n';
    salida.close();
    if (!salida) return false;
    std::remove(ruta.c_str());
    return std::rename(temporal.c_str(), ruta.c_str()) == 0;
}
