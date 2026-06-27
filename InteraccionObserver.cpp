#include "InteraccionObserver.h"

ObservadorGenerosFavoritos::ObservadorGenerosFavoritos(std::map<std::string, int>* frecuencias)
    : frecuenciaGeneros(frecuencias) {}

void ObservadorGenerosFavoritos::actualizar(const EventoInteraccion& evento) {
    if (!frecuenciaGeneros) return;

    if (evento.accion == "LIKE_AGREGADO") {
        (*frecuenciaGeneros)[evento.pelicula.genero]++;
    } else if (evento.accion == "LIKE_REMOVIDO") {
        (*frecuenciaGeneros)[evento.pelicula.genero]--;
        if ((*frecuenciaGeneros)[evento.pelicula.genero] <= 0) {
            frecuenciaGeneros->erase(evento.pelicula.genero);
        }
    }
}

ObservadorHistorialInteracciones::ObservadorHistorialInteracciones()
    : totalLikes(0), totalVerMasTarde(0) {}

void ObservadorHistorialInteracciones::actualizar(const EventoInteraccion& evento) {
    if (evento.accion == "LIKE_AGREGADO") {
        totalLikes++;
    } else if (evento.accion == "LIKE_REMOVIDO" && totalLikes > 0) {
        totalLikes--;
    } else if (evento.accion == "VMT_AGREGADO") {
        totalVerMasTarde++;
    } else if (evento.accion == "VMT_REMOVIDO" && totalVerMasTarde > 0) {
        totalVerMasTarde--;
    }
}

int ObservadorHistorialInteracciones::getTotalLikes() const {
    return totalLikes;
}

int ObservadorHistorialInteracciones::getTotalVerMasTarde() const {
    return totalVerMasTarde;
}
