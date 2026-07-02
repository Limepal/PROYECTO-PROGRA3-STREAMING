#ifndef OBSERVADOR_PERFIL_H
#define OBSERVADOR_PERFIL_H

#include <iostream>
#include <string>

enum class EventoPerfil { LikeAgregado, LikeEliminado, PendienteAgregado, PendienteEliminado };

// Patron Observer: los consumidores reaccionan a cambios del perfil sin
// acoplarse a la logica que modifica sus colecciones.
class ObservadorPerfil {
public:
    virtual ~ObservadorPerfil() = default;
    virtual void actualizar(EventoPerfil evento, const std::string& titulo) = 0;
};

class NotificadorConsola final : public ObservadorPerfil {
public:
    void actualizar(EventoPerfil evento, const std::string& titulo) override {
        const char* mensaje = "";
        switch (evento) {
            case EventoPerfil::LikeAgregado:       mensaje = "Like agregado: "; break;
            case EventoPerfil::LikeEliminado:      mensaje = "Like removido: "; break;
            case EventoPerfil::PendienteAgregado:  mensaje = "Agregado a Ver mas tarde: "; break;
            case EventoPerfil::PendienteEliminado: mensaje = "Removido de Ver mas tarde: "; break;
        }
        std::cout << mensaje << titulo << '\n';
    }
};

#endif
