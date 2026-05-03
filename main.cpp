#include <iostream>
#include "PreprocesamientoDatos.h"
#include "MotorBusqueda.h"

using namespace std;

int main() {
    MotorBusqueda motor;

    cout << "--- Fase 1: Limpieza y Carga de Datos ---" << endl;
    vector<DatosPelicula> datosSucios = CargarYLimpiarDatos("wiki_movie_plots_deduped.csv");

    cout << "--- Fase 2: Indexación en el Motor de Búsqueda ---" << endl;
    int i = 0;
    for (const auto& d : datosSucios) {
        Pelicula p;
        p.year = d.year;
        p.titulo = d.titulo;
        p.genero = d.genero;
        p.trama = d.trama;
        // El motor se encarga de insertarla en el Trie e Índice Invertido
        motor.agregarPelicula(p);
        i ++;
        cout << "PelIndEx!: " << i << " --";
    }
    motor.finalizarIndexacion();
    cout << "Indexación completada. " << datosSucios.size() << " películas listas." << endl;

    // Ejemplo de uso del Buscador
    cout << "\n--- Fase 3: Pruebas de Búsqueda ---" << endl;

    // Prueba de búsqueda por título (Trie)
    string busquedaTitulo = "kansas saloon";
    cout << "Buscando titulo: '" << busquedaTitulo << "'..." << endl;
    vector<int> resultadosTrie = motor.buscarPorTitulo(busquedaTitulo);

    for (int id : resultadosTrie) {
        Pelicula res = motor.obtenerPelicula(id);
        cout << "[Encontrado] (" << res.year << ") " << res.titulo << endl;
    }

    // Prueba de búsqueda por trama (Índice Invertido)
    string palabraTrama = "bartender";
    cout << "\nBuscando palabra clave en tramas: '" << palabraTrama << "'..." << endl;
    vector<int> resultadosTrama = motor.buscarEnTrama(palabraTrama);

    for (int id : resultadosTrama) {
        Pelicula res = motor.obtenerPelicula(id);
        cout << "[Menciona '" << palabraTrama << "']: " << res.titulo << endl;
    }

    return 0;
}