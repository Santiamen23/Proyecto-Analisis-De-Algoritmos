#pragma once

#include <vector>

#include "graph.h"

using namespace std;

struct ResultadoAlcance {
    int nodeIdOrigen;
    int cantidadAlcanzable;
    double porcentaje;
};

struct ResultadoComponentes {
    int cantidadComponentes;
    int tamanoComponenteGigante;
    int idComponenteGigante;
    vector<int> componentePorNodo;
    vector<vector<int>> componentes;
};

struct ResultadoDiametro {
    int nodeIdOrigen;
    int nodeIdDestino;
    double distanciaMetros;
};

struct ResultadoMst {
    int tamanoComponenteGigante{};
    int aristasUsadas;
    double distanciaTotalMetros;
};

struct DatosRuta {
    vector<int> recorridoNodeIds;
    double distanciaMetros;
    double tiempoSegundos;
};

struct ResultadoRutas {
    int nodeIdOrigen;
    int nodeIdDestino;
    DatosRuta rutaPorDistancia;
    DatosRuta rutaPorTiempo;
};

class AlgoritmosGrafo {
private:
    struct ResultadoDijkstra {
        vector<double> distancias;
        vector<int> padre;
        vector<int> indiceAristaPadre;
    };

    const Grafo* grafo;

public:
    explicit AlgoritmosGrafo(const Grafo* grafo);

    ResultadoAlcance calcularAlcanceVehicular(int nodeIdOrigen, double limiteMetros = 5000.0) const;
    ResultadoComponentes calcularComponentesDebilmenteConexas() const;
    ResultadoDiametro calcularDiametroVial() const;
    ResultadoMst calcularRedEmergenciaMinima() const;
    ResultadoRutas compararRutas(int nodeIdOrigen, int nodeIdDestino) const;

private:
    ResultadoDijkstra dijkstra(int indiceOrigen, const ListaAdyacencia& adyacencia) const;
    DatosRuta construirDatosRuta(int indiceOrigen, int indiceDestino, const ResultadoDijkstra& resultado) const;
    double calcularTiempoAristaSegundos(const Arista& arista) const;
};
