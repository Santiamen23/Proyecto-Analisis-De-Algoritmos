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

    Grafo* grafo;

public:
    AlgoritmosGrafo(Grafo* grafo);

    ResultadoAlcance calcularAlcanceVehicular(int nodeIdOrigen, double limiteMetros = 5000.0);
    ResultadoComponentes calcularComponentesDebilmenteConexas();
    ResultadoDiametro calcularDiametroVial();
    ResultadoMst calcularRedEmergenciaMinima();
    ResultadoRutas compararRutas(int nodeIdOrigen, int nodeIdDestino);

private:
    ResultadoDijkstra dijkstra(int indOrigen, vector<vector<EntradaAdyacencia>> adyacencia);
    DatosRuta construirDatosRuta(int indOrigen, int indDestino, ResultadoDijkstra resultado);
    double calcularTiempoAristaSegundos(Arista arista);
};
