#include "Interface.h"
#include <iostream>
#include <vector>
using namespace std;

Interface::Interface(MotorBusqueda& m) : motor(m) {}


// ===================== INICIO =====================
void Interface::iniciar() {
    cout << "\n=====================================\n";
    cout << "   PLATAFORMA DE STREAMING MOVIES   \n";
    cout << "=====================================\n";

    menuPrincipal();
}


// ===================== MENU =====================
void Interface::menuPrincipal() {
    int op;

    do {
        cout << "\n--- MENU PRINCIPAL ---\n";
        cout << "1. Buscar pelicula\n";
        cout << "2. Ver 'Ver mas tarde'\n";
        cout << "3. Ver recomendaciones\n";
        cout << "4. Salir\n";
        cout << "Opcion: ";
        cin >> op;
        cin.ignore();

        switch (op) {
            case 1: buscarPeliculas(); break;
            case 2: verVerMasTarde(); break;
            case 3: verRecomendaciones(); break;
            case 4: cout << "Saliendo...\n"; break;
            default: cout << "Opcion invalida\n";
        }

    } while (op != 4);
}


// ===================== BUSQUEDA =====================
void Interface::buscarPeliculas() {
    string query;

    cout << "\nIngrese palabra o frase: ";
    getline(cin, query);

    vector<int> resultados = motor.buscarEnTrama(query);

    if (resultados.empty()) {
        cout << "No se encontraron resultados.\n";
        return;
    }

    mostrarPeliculas(resultados, 0);
}


// ===================== MOSTRAR PAGINADO =====================
void Interface::mostrarPeliculas(vector<int>& ids, int inicio) {
    int i = inicio;
    char opcion;

    while (i < ids.size()) {

        cout << "\n--- RESULTADOS ---\n";

        int limite = min(i + 5, (int)ids.size());

        for (int j = i; j < limite; j++) {
            Pelicula p = motor.obtenerPelicula(ids[j]);
            cout << j << ". " << p.titulo << " (" << p.year << ")\n";
        }

        cout << "\n[n] siguiente | [0-4] ver pelicula | [b] volver\n";
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 'n') {
            i += 5;
        }
        else if (opcion == 'b') {
            return;
        }
        else if (isdigit(opcion)) {

            int idx = opcion - '0';

            if (idx >= 0 && idx < ids.size()) {
                verDetallePelicula(ids[idx]);
            }
        }
    }
}


// ===================== DETALLE PELICULA =====================
void Interface::verDetallePelicula(int id) {
    Pelicula p = motor.obtenerPelicula(id);

    cout << "\n========== DETALLE ==========\n";
    cout << "Titulo: " << p.titulo << "\n";
    cout << "Año: " << p.year << "\n";
    cout << "Genero: " << p.genero << "\n";
    cout << "Sinopsis: " << p.trama << "\n";

    int op;
    cout << "\n1. Like\n2. Ver mas tarde\n3. Volver\n";
    cout << "Opcion: ";
    cin >> op;

    if (op == 1) agregarLike(id);
    else if (op == 2) agregarVerMasTarde(id);
}


// ===================== LIKE =====================
void Interface::agregarLike(int id) {
    likes.push_back(id);
    cout << "Agregado a likes\n";
}


// ===================== VER MAS TARDE =====================
void Interface::agregarVerMasTarde(int id) {
    verMasTarde.push_back(id);
    cout << "Agregado a Ver mas tarde\n";
}


// ===================== LISTA VER MAS TARDE =====================
void Interface::verVerMasTarde() {

    if (verMasTarde.empty()) {
        cout << "Lista vacia\n";
        return;
    }

    cout << "\n--- VER MAS TARDE ---\n";

    for (int id : verMasTarde) {
        Pelicula p = motor.obtenerPelicula(id);
        cout << "- " << p.titulo << "\n";
    }
}


// ===================== RECOMENDACIONES (BASICO) =====================
// versión simple: usa likes y recomienda por género
void Interface::verRecomendaciones() {

    if (likes.empty()) {
        cout << "No hay likes aun para recomendar.\n";
        return;
    }

    cout << "\n--- RECOMENDACIONES ---\n";

    string generoBase = motor.obtenerPelicula(likes.back()).genero;

    int contador = 0;

    for (int i = 0; i < 10000 && contador < 5; i++) {
        Pelicula p = motor.obtenerPelicula(i);

        if (p.genero == generoBase) {
            cout << "- " << p.titulo << " (" << p.genero << ")\n";
            contador++;
        }
    }
}