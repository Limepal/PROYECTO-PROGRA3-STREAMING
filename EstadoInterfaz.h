#ifndef ESTADO_INTERFAZ_H
#define ESTADO_INTERFAZ_H

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
