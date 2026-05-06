#ifndef INTERFACE_H
#define INTERFACE_H

#include "MotorBusqueda.h"
#include <vector>
#include <string>

using namespace std;

class Interface {
private:
    MotorBusqueda& motor;

    vector<int> likes;
    vector<int> verMasTarde;

    // utilidades internas
    void mostrarPeliculas(vector<int>& ids, int inicio);
    void verDetallePelicula(int id);
    void agregarLike(int id);
    void agregarVerMasTarde(int id);

public:
    Interface(MotorBusqueda& m);

    void iniciar();
    void menuPrincipal();

    void buscarPeliculas();
    void verVerMasTarde();
    void verRecomendaciones();

    ~Interface() = default;
};

#endif //INTERFACE_H