#ifndef INTERACCION_OBSERVER_H
#define INTERACCION_OBSERVER_H

#include "MotorBusqueda.h"
#include <map>
#include <string>

// ============================================================
// PATRON OBSERVER
// ============================================================
// Los observadores reaccionan a eventos del usuario sin que la
// interfaz tenga que conocer todos los efectos secundarios.

struct EventoInteraccion {
    int idPelicula;
    Pelicula pelicula;
    std::string accion; // LIKE_AGREGADO, LIKE_REMOVIDO, VMT_AGREGADO, VMT_REMOVIDO
};

class IInteraccionObserver {
public:
    virtual ~IInteraccionObserver() = default;
    virtual void actualizar(const EventoInteraccion& evento) = 0;
};

class ObservadorGenerosFavoritos : public IInteraccionObserver {
private:
    std::map<std::string, int>* frecuenciaGeneros;

public:
    explicit ObservadorGenerosFavoritos(std::map<std::string, int>* frecuencias);
    void actualizar(const EventoInteraccion& evento) override;
};

class ObservadorHistorialInteracciones : public IInteraccionObserver {
private:
    int totalLikes;
    int totalVerMasTarde;

public:
    ObservadorHistorialInteracciones();
    void actualizar(const EventoInteraccion& evento) override;
    int getTotalLikes() const;
    int getTotalVerMasTarde() const;
};

#endif
