#ifndef ESTRATEGIA_RELEVANCIA_H
#define ESTRATEGIA_RELEVANCIA_H
#include "MotorBusqueda.h"
#include <map>
class EstrategiaRelevancia {
public:
    virtual ~EstrategiaRelevancia() = default;
    virtual double calcular(const Pelicula&, const std::vector<std::string>&,
                            bool, bool, bool, bool,
                            const std::map<std::string, int>&) const = 0;
};
class RelevanciaPonderada final : public EstrategiaRelevancia {
public:
    double calcular(const Pelicula& p, const std::vector<std::string>& terminos,
                    bool titulo, bool trama, bool tag, bool pendiente,
                    const std::map<std::string, int>& generos) const override {
        double score = (titulo ? 3.0 : 0.0) + (trama ? 2.0 : 0.0) + (tag ? 2.5 : 0.0);
        for (const auto& t : terminos) if (p.titulo.find(t) == 0) score += 1.0;
        try {
            int year = std::stoi(p.year);
            score += year >= 2015 ? .5 : year >= 2010 ? .4 : year >= 2000 ? .2 : year >= 1990 ? .1 : 0;
        } catch (...) {}
        if (pendiente) score += .2;
        auto it = generos.find(p.genero);
        if (it != generos.end()) score += .1 * it->second;
        return score;
    }
};
#endif
