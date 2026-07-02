#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include "PreprocesamientoDatos.h"
#include "MotorBusqueda.h"
#include "InterfazStreaming.h"

using namespace std;



int main() {
    MotorBusqueda motor;
    


    //Inicio de medicion de tiempo
    auto inicioProcesamiento = chrono::steady_clock::now();

    cout << "PiliFlix - Cargando base de datos..." << endl;
    ////////

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

    // Fase 2: indexacion paralela sin mutex global.
    unsigned int numHilos = thread::hardware_concurrency();
    if (numHilos == 0) numHilos = 4;
    numHilos = min(numHilos, 8u);
    numHilos = min(numHilos, static_cast<unsigned int>(total));
    size_t bloque = (total + numHilos - 1) / numHilos;

    motor.prepararCarga(total, numHilos);

    vector<vector<ParPalabraId>> buffersLocales(numHilos);
    vector<future<void>> futuros;

    for (unsigned int h = 0; h < numHilos; h++) {
        size_t inicio = h * bloque;
        size_t fin = min(inicio + bloque, total);
        futuros.push_back(async(launch::async, [&motor, &datos, &buffersLocales, inicio, fin, h]() {
            for (size_t i = inicio; i < fin; i++) {
                Pelicula p;
                p.year = datos[i].year;
                p.titulo = datos[i].titulo;
                p.genero = datos[i].genero;
                p.trama = datos[i].trama;
                p.director = datos[i].director;
                p.reparto = datos[i].reparto;
                p.origen = datos[i].origen;
                motor.agregarPeliculaConcurrente(
                    p, static_cast<int>(i), h, buffersLocales[h]);
            }
            sort(buffersLocales[h].begin(), buffersLocales[h].end());
        }));
    }
    for (auto& f : futuros) f.get();
    cout << "Indexacion paralela completada con "
         << numHilos << " trabajadores." << endl;

    motor.finalizarIndexacionParalela(buffersLocales);

    //Fin de medicion de tiempo
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
    /////

    // Fase 3: Iniciar interfaz
    InterfazStreaming app(&motor);
    app.ejecutar();

    return 0;
}
