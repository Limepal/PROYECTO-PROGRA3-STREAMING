#include <iostream>
#include <vector>
#include "PreprocesamientoDatos.h"
#include "MotorBusqueda.h"
#include "InterfazStreaming.h"

using namespace std;

int main() {
    MotorBusqueda motor;

    cout << "Piliflix- Cargando base de datos..." << endl;

    // Fase 1: Carga de datos
    LimpiarDatos("wiki_movie_plots_deduped.csv", "peliculasLimpias.csv");
    vector<DatosPelicula> datos = CargarDatosLimpios("peliculasLimpias.csv");
    if (datos.empty()) {
        cerr << "Error: No se pudo cargar wiki_movie_plots_deduped.csv" << endl;
        cerr << "Verifica que el archivo este en el directorio actual." << endl;
        return 1;
    }

    cout << datos.size() << " peliculas cargadas. Indexando..." << endl;

    // Fase 2: Indexacion
    for (size_t i = 0; i < datos.size(); i++) {
        Pelicula p;
        p.year = datos[i].year;
        p.titulo = datos[i].titulo;
        p.genero = datos[i].genero;
        p.trama = datos[i].trama;
        p.director = datos[i].director;
        p.reparto = datos[i].reparto;
        p.origen = datos[i].origen;
        motor.agregarPelicula(p);

        if ((i + 1) % 1000 == 0 || i == datos.size() - 1) {
            cout << "  Progreso: " << (i + 1) << "/" << datos.size()
                 << " (" << (100 * (i + 1) / datos.size()) << "%)";
            cout.flush();
        }
    }
    motor.finalizarIndexacion();

    cout << "Indexacion completada." << endl;
    cout << "Presiona ENTER para iniciar...";
    cin.get();

    // Fase 3: Iniciar interfaz
    InterfazStreaming app(&motor);
    app.ejecutar();

    return 0;
}