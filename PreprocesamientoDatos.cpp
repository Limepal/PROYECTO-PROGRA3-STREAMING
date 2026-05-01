#include "PreprocesamientoDatos.h"
#include <fstream>
#include <vector>
#include <cctype>

using namespace std;

// Función para limpiar texto y preparar para CSV
string procesarCadena(const string& sucia) {
    string limpia;
    limpia.reserve(sucia.size());

    bool ultimoFueEspacio = false;

    for (char c : sucia) {
        // 1. Convertir a minúscula
        char n = (char)tolower((unsigned char)c);

        // 2. Solo alfanuméricos y espacios
        // Mantenemos las comas internas si quieres, pero por seguridad
        // para un motor de búsqueda, es mejor solo alfanuméricos.
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