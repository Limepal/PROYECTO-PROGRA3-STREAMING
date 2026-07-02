#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include "PreprocesamientoDatos.h"
#include "MotorBusqueda.h"
#include "InterfazStreaming.h"

using namespace std;



int main() {
    MotorBusqueda motor;

    // Mide limpieza, carga e indexacion completas.
    auto inicioProcesamiento = chrono::steady_clock::now();

    cout << "PiliFlix - Cargando base de datos..." << endl;

    // Fase 1: Carga de datos
    LimpiarDatos("wiki_movie_plots_deduped.csv", "peliculasLimpias.csv");
    vector<DatosPelicula> datos = CargarDatosLimpios("peliculasLimpias.csv");
    if (datos.empty()) {
        cerr << "Error: No se pudo cargar wiki_movie_plots_deduped.csv" << endl;
        cerr << "Verifica que el archivo este en el directorio actual." << endl;
        return 1;
    }

    size_t total = datos.size();
    cout << total << " peliculas cargadas. Indexando..." << endl;

    // Fase 2: indexacion completamente secuencial.
    for (size_t i = 0; i < total; ++i) {
        Pelicula p;
        p.year = datos[i].year;
        p.titulo = datos[i].titulo;
        p.genero = datos[i].genero;
        p.trama = datos[i].trama;
        p.director = datos[i].director;
        p.reparto = datos[i].reparto;
        p.origen = datos[i].origen;
        motor.agregarPelicula(p);

        if ((i + 1) % 1000 == 0 || i + 1 == total) {
            int porcentaje = static_cast<int>(100 * (i + 1) / total);
            cout << "\rIndexando: " << porcentaje
                 << "% (" << (i + 1) << "/" << total << ")" << flush;
        }
    }
    cout << endl;
    motor.finalizarIndexacion();

    // El menu y la espera del usuario no forman parte de la medicion.
    auto finProcesamiento = chrono::steady_clock::now();

    double tiempoMs = chrono::duration<double, milli>(
        finProcesamiento - inicioProcesamiento
    ).count();

    double tiempoSegundos = tiempoMs / 1000.0;

    cout << fixed << setprecision(3);
    cout << "Tiempo total de procesamiento: "
         << tiempoMs << " ms ("
         << tiempoSegundos << " segundos)" << endl;

    cout << "Indexacion completada (" << motor.getTotalPeliculas() << " peliculas)." << endl;
    cout << "Presiona ENTER para iniciar...";
    cin.get();

    // Fase 3: Iniciar interfaz
    InterfazStreaming app(&motor);
    app.ejecutar();

    return 0;
}
