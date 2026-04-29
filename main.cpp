#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;


int main(){
    ifstream movieDf;
    movieDf.open("wiki_movie_plots_deduped.csv");
    string line;

    /*
    if (movieDf.is_open()) {
        while (getline(movieDf, line)) { // Leer línea por línea
            stringstream ss(line);
            string cell;

            while (getline(ss, cell, ',')) { // Separar por comas
                cout << cell << " | ";
            }
            cout << endl;
        }
        movieDf.close(); // Cerrar el archivo
    } else {
        cerr << "No se pudo abrir el archivo" << endl;
    }
    return 0;
    */
}