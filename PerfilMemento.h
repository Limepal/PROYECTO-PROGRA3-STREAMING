#ifndef PERFIL_MEMENTO_H
#define PERFIL_MEMENTO_H

#include <string>
#include <vector>

// Patron Memento: captura el estado persistible del perfil sin exponer
// detalles del mecanismo de almacenamiento a quien lo restaura.
class PerfilMemento {
    std::vector<int> likes;
    std::vector<int> verMasTarde;
public:
    PerfilMemento(std::vector<int> likesGuardados,
                  std::vector<int> pendientesGuardados);
    const std::vector<int>& obtenerLikes() const;
    const std::vector<int>& obtenerVerMasTarde() const;
};

// Caretaker del patron Memento.
class CuidadorPerfil {
    std::string ruta;
public:
    explicit CuidadorPerfil(std::string rutaArchivo);
    bool guardar(const PerfilMemento& memento) const;
    bool existe() const;
    PerfilMemento restaurar() const;
};

#endif
