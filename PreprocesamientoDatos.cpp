// PreprocesamientoDatos.cpp

#include "PreprocesamientoDatos.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <thread>
#include <future>

using namespace std;

// =========================
// STOPWORDS
// =========================

static const unordered_set<string> stopwords = {
    "the", "a", "an", "of", "in", "on", "at",
    "and", "to", "for", "with"
};

// =========================
// TRIM
// =========================

string trim(const string& s) {
    size_t inicio = s.find_first_not_of(' ');
    size_t fin = s.find_last_not_of(' ');

    if (inicio == string::npos)
        return "";

    return s.substr(inicio, fin - inicio + 1);
}

// =========================
// LIMPIEZA DE TEXTO
// =========================

string procesarCadena(const string& sucia) {
    string limpia;
    limpia.reserve(sucia.size());

    bool ultimoFueEspacio = false;

    for (char c : sucia) {

        char n = (char)tolower((unsigned char)c);

        // Permitimos:
        // - letras/números
        // - espacios
        // - guiones
        // - apostrofes
        if (isalnum((unsigned char)n) ||
            n == ' ' ||
            n == '-' ||
            n == '\'') {

            if (n == ' ') {

                if (!ultimoFueEspacio) {
                    limpia += n;
                    ultimoFueEspacio = true;
                }

            } else {

                limpia += n;
                ultimoFueEspacio = false;
            }
        }
    }

    return trim(limpia);
}

// =========================
// CSV PARSER
// =========================

vector<string> separarLinea(const string& linea) {

    vector<string> resultado;

    string campo;
    campo.reserve(linea.size());

    bool dentroComillas = false;

    for (size_t i = 0; i < linea.length(); ++i) {

        char c = linea[i];

        if (c == '"') {

            dentroComillas = !dentroComillas;

        } else if (c == ',' && !dentroComillas) {

            resultado.push_back(campo);
            campo.clear();

        } else {

            campo += c;
        }
    }

    resultado.push_back(campo);

    return resultado;
}

// =========================
// ELIMINAR STOPWORDS
// =========================

string eliminarStopwords(const string& texto) {

    string resultado;
    string palabra;

    for (char c : texto) {

        if (c == ' ') {

            if (!palabra.empty() &&
                stopwords.find(palabra) == stopwords.end()) {

                if (!resultado.empty())
                    resultado += ' ';

                resultado += palabra;
            }

            palabra.clear();

        } else {

            palabra += c;
        }
    }

    // Última palabra
    if (!palabra.empty() &&
        stopwords.find(palabra) == stopwords.end()) {

        if (!resultado.empty())
            resultado += ' ';

        resultado += palabra;
    }

    return resultado;
}
//parser robusto

vector<vector<string>> ParsearCSVRobusto(istream& entrada) {
    vector<vector<string>> filas;
    vector<string> fila_actual;
    string campo_actual;

    bool dentroComillas = false;
    char c;

    while (entrada.get(c)) {
        if (c == '"') {
            if (dentroComillas && entrada.peek() == '"') {
                campo_actual += '"';
                entrada.get(); // Consumir comilla doble escapada
            } else {
                dentroComillas = !dentroComillas;
            }
        }
        else if (c == ',' && !dentroComillas) {
            fila_actual.push_back(campo_actual);
            campo_actual.clear();
        }
        else if ((c == '\n' || c == '\r') && !dentroComillas) {
            if (c == '\r' && entrada.peek() == '\n') {
                entrada.get(); // Consumir \n en finales de linea Windows
            }
            if (!campo_actual.empty() || !fila_actual.empty()) {
                fila_actual.push_back(campo_actual);
                filas.push_back(fila_actual);
                fila_actual.clear();
                campo_actual.clear();
            }
        }
        else {
            campo_actual += c;
        }
    }

    if (!campo_actual.empty() || !fila_actual.empty()) {
        fila_actual.push_back(campo_actual);
        filas.push_back(fila_actual);
    }

    return filas;
}



// =========================
// LIMPIAR CSV (PARALELO)
// =========================

void LimpiarDatos(const string& nombreEntrada, const string& nombreSalida) {

    ifstream entrada(nombreEntrada);
    if (!entrada.is_open()) {
        cerr << "Error al abrir archivo de entrada.\n";
        return;
    }

    // Leer y parsear el archivo completo a memoria correctamente
    vector<vector<string>> filas = ParsearCSVRobusto(entrada);
    entrada.close();

    // Eliminar la cabecera si el archivo no está vacío
    if (!filas.empty()) {
        filas.erase(filas.begin());
    }

    if (filas.empty()) {
        ofstream salida(nombreSalida);
        salida << "Year,Title,Origin,Director,Cast,Genre,Plot\n";
        return;
    }

    // Procesar en paralelo
    unsigned int numHilos = thread::hardware_concurrency();
    if (numHilos == 0) numHilos = 4;
    size_t bloque = (filas.size() + numHilos - 1) / numHilos;

    vector<string> resultados(filas.size());
    vector<future<void>> futuros;

    for (unsigned int h = 0; h < numHilos; h++) {
        size_t inicio = h * bloque;
        size_t fin = min(inicio + bloque, filas.size());

        futuros.push_back(async(launch::async, [&filas, &resultados, inicio, fin]() {
            for (size_t i = inicio; i < fin; i++) {
                const vector<string>& campos = filas[i];

                // Mantenemos tu logica original de indices (0 al 5, y 7)
                if (campos.size() >= 8) {
                    const string& year = campos[0];
                    string titulo  = eliminarStopwords(procesarCadena(campos[1]));
                    string origen  = eliminarStopwords(procesarCadena(campos[2]));
                    string director= eliminarStopwords(procesarCadena(campos[3]));
                    string reparto = eliminarStopwords(procesarCadena(campos[4]));
                    string genero  = (campos[5] == "unknown")
                                    ? "otros"
                                    : eliminarStopwords(procesarCadena(campos[5]));
                    string trama   = eliminarStopwords(procesarCadena(campos[7]));

                    // Se aplanan las lineas procesadas (quitando \n internos si los hubiera)
                    // para que el CSV de salida sea mas sencillo de procesar despues.
                    resultados[i] = year + "," +
                        "\"" + titulo + "\"," +
                        "\"" + origen + "\"," +
                        "\"" + director + "\"," +
                        "\"" + reparto + "\"," +
                        "\"" + genero + "\"," +
                        "\"" + trama + "\"\n";
                }
            }
        }));
    }
    for (auto& f : futuros) f.get();

    // Escribir resultados secuencialmente
    ofstream salida(nombreSalida);
    if (!salida.is_open()) {
        cerr << "Error al crear archivo de salida.\n";
        return;
    }
    salida << "Year,Title,Origin,Director,Cast,Genre,Plot\n";
    for (const auto& r : resultados) {
        if (!r.empty()) salida << r;
    }
    salida.close();
}

// =========================
// CARGAR CSV LIMPIO (PARALELO)
// =========================

vector<DatosPelicula> CargarDatosLimpios(
    const string& nombreArchivo
) {
    ifstream entrada(nombreArchivo);
    if (!entrada.is_open()) {
        cerr << "Error al abrir archivo limpio.\n";
        return {};
    }

    // Leer todas las lineas a memoria
    vector<string> lineas;
    string linea;
    bool esCabecera = true;
    while (getline(entrada, linea)) {
        if (esCabecera) {
            esCabecera = false;
            continue;
        }
        lineas.push_back(move(linea));
    }
    entrada.close();

    if (lineas.empty()) return {};

    // Pre-asignar espacio
    vector<DatosPelicula> lista(lineas.size());

    // Procesar en paralelo
    unsigned int numHilos = thread::hardware_concurrency();
    if (numHilos == 0) numHilos = 4;
    size_t bloque = (lineas.size() + numHilos - 1) / numHilos;

    vector<future<void>> futuros;
    for (unsigned int h = 0; h < numHilos; h++) {
        size_t inicio = h * bloque;
        size_t fin = min(inicio + bloque, lineas.size());
        futuros.push_back(async(launch::async, [&lineas, &lista, inicio, fin]() {
            for (size_t i = inicio; i < fin; i++) {
                vector<string> campos = separarLinea(lineas[i]);
                if (campos.size() >= 7) {
                    lista[i].year     = campos[0];
                    lista[i].titulo   = campos[1];
                    lista[i].origen   = campos[2];
                    lista[i].director = campos[3];
                    lista[i].reparto  = campos[4];
                    lista[i].genero   = campos[5];
                    lista[i].trama    = campos[6];
                }
            }
        }));
    }
    for (auto& f : futuros) f.get();

    // Eliminar entradas vacias (lineas con menos de 7 campos)
    lista.erase(remove_if(lista.begin(), lista.end(),
        [](const DatosPelicula& d) { return d.year.empty(); }),
        lista.end());

    lista.shrink_to_fit();
    return lista;
}