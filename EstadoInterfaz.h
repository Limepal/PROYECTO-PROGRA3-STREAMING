//
// Created by Dayron Cueva on 30/06/2026.
//

#ifndef ESTADOINTERFAZ_H
#define ESTADOINTERFAZ_H

class InterfazStreaming;

// CLASE BASE ABSTRACTA
class EstadoInterfaz {
public:
    virtual ~EstadoInterfaz() = default;

    // Punto de entrada del estado: renderiza y procesa input.
    // Retorna false cuando la aplicacion debe terminar.
    virtual bool ejecutar(InterfazStreaming& ctx) = 0;

    // Nombre descriptivo (util para debug/documentacion)
    virtual const char* nombre() const = 0;
};

#endif // ESTADO_INTERFAZ_H