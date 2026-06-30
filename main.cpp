#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <chrono>
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

    size_t total = datos.size();
    cout << total << " peliculas cargadas. Indexando..." << endl;

    // Fase 2: Indexacion paralela con barra de progreso
    unsigned int numHilos = thread::hardware_concurrency();
    if (numHilos == 0) numHilos = 4;
    size_t bloque = (total + numHilos - 1) / numHilos;

    atomic<size_t> progreso{0};

    // Hilo dedicado a dibujar la barra de progreso
    thread hiloProgreso([&]() {
        size_t ultimo = 0;
        while (progreso < total) {
            size_t actual = progreso;
            if (actual != ultimo) {
                int pct = (int)(100 * actual / total);
                cout << "\r[";
                int pos = pct / 5;
                for (int i = 0; i < 20; i++) {
                    cout << (i < pos ? '=' : (i == pos ? '>' : ' '));
                }
                cout << "] " << pct << "% (" << actual << "/" << total << ")   ";
                cout.flush();
                ultimo = actual;
            }
            this_thread::sleep_for(chrono::milliseconds(80));
        }
        // 100% final
        cout << "\r[";
        for (int i = 0; i < 20; i++) cout << '=';
        cout << "] 100% (" << total << "/" << total << ")   " << endl;
    });

    vector<vector<ParPalabraId>> buffersLocales(numHilos);
    vector<future<void>> futuros;

    for (unsigned int h = 0; h < numHilos; h++) {
        size_t inicio = h * bloque;
        size_t fin = min(inicio + bloque, total);
        futuros.push_back(async(launch::async, [&motor, &datos, &buffersLocales, &progreso, inicio, fin, h]() {
            for (size_t i = inicio; i < fin; i++) {
                Pelicula p;
                p.year = datos[i].year;
                p.titulo = datos[i].titulo;
                p.genero = datos[i].genero;
                p.trama = datos[i].trama;
                p.director = datos[i].director;
                p.reparto = datos[i].reparto;
                p.origen = datos[i].origen;
                motor.agregarPeliculaConcurrente(p, buffersLocales[h]);
                progreso++;
            }
        }));
    }
    for (auto& f : futuros) f.get();
    hiloProgreso.join();

    motor.mergeBuffers(buffersLocales);
    motor.finalizarIndexacion();

    cout << "Indexacion completada (" << motor.getTotalPeliculas() << " peliculas)." << endl;
    cout << "Presiona ENTER para iniciar...";
    cin.get();

    // Fase 3: Iniciar interfaz
    InterfazStreaming app(&motor);
    app.ejecutar();

    return 0;
}