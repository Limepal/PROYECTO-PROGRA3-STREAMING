#include "PreprocesamientoDatos.h"
#include <fstream>
#include <vector>

using namespace std;

// Función para limpiar texto y preparar para CSV
string procesarCadena(const string& sucia) {
    string limpia;
    limpia.reserve(sucia.size());
    //reserve preasigna un espacio de memoria vacio para un contenedor de tamaño res

    bool ultimoFueEspacio = false;

    for (char c : sucia) {
        // Convertir a minuscula
        char n = (char)tolower((unsigned char)c);

        // Se mantienen solo alfanumericos y espacios, y las comas por practicidad
        if (isalnum((unsigned char)n) || n == ' ') {
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
    return limpia;
}

// Función para separar CSV original (basada en comillas)
vector<string> separarLinea(const string& linea) {
    vector<string> resultado;
    string campo;
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

void LimpiarDatos(string nombreEntrada, string nombreSalida) {
    ifstream entrada(nombreEntrada);
    ofstream salida(nombreSalida);

    if (!entrada.is_open() || !salida.is_open()) return;

    string linea;
    bool esCabecera = true;

    while (getline(entrada, linea)) {
        if (esCabecera) {
            // Cabecera estándar separada por comas
            salida << "Year,Title,Origin,Director,Cast,Genre,Plot\n";
            esCabecera = false;
            continue;
        }

        vector<string> campos = separarLinea(linea);

        if (campos.size() >= 8) {
            string year     = campos[0];
            string titulo   = procesarCadena(campos[1]);
            string origen   = procesarCadena(campos[2]);
            string director = procesarCadena(campos[3]);
            string reparto  = procesarCadena(campos[4]);
            string genero   = (campos[5] == "unknown") ? "otros" : procesarCadena(campos[5]);
            string trama    = procesarCadena(campos[7]);

            // IMPORTANTE: Envolvemos cada campo de texto en comillas ""
            // para que las comas del formato no se confundan con el contenido.
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

vector<DatosPelicula> CargarYLimpiarDatos(string nombreArchivo) {
    vector<DatosPelicula> lista;
    // OPTIMIZACIÓN: Reservamos espacio para evitar reasignaciones costosas
    // 35000 es un estimado bajo para este dataset, ajusta según necesites
    lista.reserve(35000);

    ifstream entrada(nombreArchivo);
    string linea;
    bool esCabecera = true;

    while (getline(entrada, linea)) {
        if (esCabecera) { esCabecera = false; continue; }

        vector<string> campos = separarLinea(linea);
        if (campos.size() >= 8) {
            DatosPelicula d;
            d.year     = campos[0];
            d.titulo   = procesarCadena(campos[1]);
            d.origen   = procesarCadena(campos[2]);
            d.director = procesarCadena(campos[3]);
            d.reparto  = procesarCadena(campos[4]);
            d.genero   = (campos[5] == "unknown") ? "otros" : procesarCadena(campos[5]);
            d.trama    = procesarCadena(campos[7]);
            lista.push_back(d);
        }
    }
    return lista;
}