// PreprocesamientoDatos.cpp

#include "PreprocesamientoDatos.h"

#include <fstream>
#include <iostream>
#include <unordered_set>

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

// =========================
// LIMPIAR CSV
// =========================

void LimpiarDatos(const string& nombreEntrada,
                  const string& nombreSalida) {

    ifstream entrada(nombreEntrada);
    ofstream salida(nombreSalida);

    if (!entrada.is_open()) {
        cerr << "Error al abrir archivo de entrada.\n";
        return;
    }

    if (!salida.is_open()) {
        cerr << "Error al crear archivo de salida.\n";
        return;
    }

    string linea;
    bool esCabecera = true;

    while (getline(entrada, linea)) {

        if (esCabecera) {

            salida << "Year,Title,Origin,Director,Cast,Genre,Plot\n";

            esCabecera = false;
            continue;
        }

        vector<string> campos = separarLinea(linea);

        if (campos.size() >= 8) {

            const string& year = campos[0];

            string titulo   =
                eliminarStopwords(
                    procesarCadena(campos[1])
                );

            string origen   =
                eliminarStopwords(
                    procesarCadena(campos[2])
                );

            string director =
                eliminarStopwords(
                    procesarCadena(campos[3])
                );

            string reparto  =
                eliminarStopwords(
                    procesarCadena(campos[4])
                );

            string genero =
                (campos[5] == "unknown")
                ? "otros"
                : eliminarStopwords(
                    procesarCadena(campos[5])
                );

            string trama =
                eliminarStopwords(
                    procesarCadena(campos[7])
                );

            salida << year << ","
                   << "\"" << titulo << "\","
                   << "\"" << origen << "\","
                   << "\"" << director << "\","
                   << "\"" << reparto << "\","
                   << "\"" << genero << "\","
                   << "\"" << trama << "\"\n";
        }
    }

    entrada.close();
    salida.close();
}

// =========================
// CARGAR CSV LIMPIO
// =========================

vector<DatosPelicula> CargarDatosLimpios(
    const string& nombreArchivo
) {

    vector<DatosPelicula> lista;

    lista.reserve(35000);

    ifstream entrada(nombreArchivo);

    if (!entrada.is_open()) {
        cerr << "Error al abrir archivo limpio.\n";
        return lista;
    }

    string linea;
    bool esCabecera = true;

    while (getline(entrada, linea)) {

        if (esCabecera) {
            esCabecera = false;
            continue;
        }

        vector<string> campos = separarLinea(linea);

        if (campos.size() >= 7) {

            DatosPelicula d;

            d.year     = campos[0];
            d.titulo   = campos[1];
            d.origen   = campos[2];
            d.director = campos[3];
            d.reparto  = campos[4];
            d.genero   = campos[5];
            d.trama    = campos[6];

            lista.emplace_back(d);
        }
    }

    entrada.close();

    return lista;
}