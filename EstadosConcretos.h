#ifndef ESTADOS_CONCRETOS_H
#define ESTADOS_CONCRETOS_H
#include "EstadoInterfaz.h"
#include "InterfazStreaming.h"
#include <sstream>


// EstadoInicio
class EstadoInicio : public EstadoInterfaz {
public:
    bool ejecutar(InterfazStreaming& ctx) override {
        ctx.pantallaInicio();
        ctx.irAlMenu();   // InterfazStreaming hace new EstadoMenu()
        return true;
    }
    const char* nombre() const override { return "Inicio"; }
};


// EstadoBusqueda
class EstadoBusqueda : public EstadoInterfaz {
public:
    bool ejecutar(InterfazStreaming& ctx) override {
        ctx.pantallaBusqueda();
        ctx.irAlMenu();
        return true;
    }
    const char* nombre() const override { return "Busqueda"; }
};


// EstadoVerMasTarde
class EstadoVerMasTarde : public EstadoInterfaz {
public:
    bool ejecutar(InterfazStreaming& ctx) override {
        ctx.pantallaVerMasTarde();
        ctx.irAlMenu();
        return true;
    }
    const char* nombre() const override { return "VerMasTarde"; }
};


// EstadoMisLikes
class EstadoMisLikes : public EstadoInterfaz {
public:
    bool ejecutar(InterfazStreaming& ctx) override {
        ctx.pantallaMisLikes();
        ctx.irAlMenu();
        return true;
    }
    const char* nombre() const override { return "MisLikes"; }
};


// EstadoEstadisticas
class EstadoEstadisticas : public EstadoInterfaz {
public:
    bool ejecutar(InterfazStreaming& ctx) override {
        ctx.pantallaEstadisticas();
        ctx.irAlMenu();
        return true;
    }
    const char* nombre() const override { return "Estadisticas"; }
};


// EstadoDetallePelicula
class EstadoDetallePelicula : public EstadoInterfaz {
    int peliculaId;
public:
    explicit EstadoDetallePelicula(int id) : peliculaId(id) {}
    bool ejecutar(InterfazStreaming& ctx) override {
        ctx.mostrarDetalle(peliculaId);
        ctx.irAlMenu();
        return true;
    }
    const char* nombre() const override { return "DetallePelicula"; }
};


// EstadoMenu  (ultimo: el switch usa "new" sobre clases ya definidas)
class EstadoMenu : public EstadoInterfaz {
public:
    bool ejecutar(InterfazStreaming& ctx) override {
        ctx.titulo("PILIFLIX - MENU PRINCIPAL");

        cout << "  [1] Buscar peliculas\n"
                "  [2] Ver mas tarde\n"
                "  [3] Mis Likes\n"
                "  [4] Estadisticas\n"
                "  [5] Pantalla de inicio\n"
                "  [0] Salir\n"
                "============================================================\n"
                "Selecciona: "
             << flush;

        string entrada;

        if (!getline(cin >> ws, entrada)) {
            return false;
        }

        istringstream flujo(entrada);
        int op;
        char sobrante;

        if (!(flujo >> op) || (flujo >> sobrante)) {
            cout << "Opcion invalida." << endl;
            ctx.pausar();
            return true;
        }

        switch (op) {
            case 1: ctx.cambiarEstado(new EstadoBusqueda());     break;
            case 2: ctx.cambiarEstado(new EstadoVerMasTarde());  break;
            case 3: ctx.cambiarEstado(new EstadoMisLikes());     break;
            case 4: ctx.cambiarEstado(new EstadoEstadisticas()); break;
            case 5: ctx.cambiarEstado(new EstadoInicio());       break;
            case 0:
                ctx.titulo("HASTA PRONTO");
                cout << "Gracias por usar PiliFlix." << endl;
                return false;
            default:
                cout << "Opcion invalida." << endl;
                ctx.pausar();
        }
        return true;
    }

    const char* nombre() const override { return "Menu"; }

};

#endif // ESTADOS_CONCRETOS_H
