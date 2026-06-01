#pragma once

#include <unordered_map>
#include <vector>

#include "models.h"

using namespace std;

using ListaAdyacencia = vector<vector<AristaAdyacente>>;

class Grafo {
private:
    vector<Nodo> nodos;
    vector<Arista> aristas;
    ListaAdyacencia adyacenciaDistancia;
    ListaAdyacencia adyacenciaTiempo;
    ListaAdyacencia adyacenciaNoDirigida;
    unordered_map<int, int> indicePorNodeId;

    void reconstruirIndiceNodos();
    void inicializarListasAdyacencia();
    void registrarAristaDirigida(int indiceOrigen, int indiceDestino, double distanciaMetros, double tiempoSegundos, int indiceArista);
    void registrarAristaNoDirigida(int indiceOrigen, int indiceDestino, double distanciaMetros, int indiceArista);

public:
    void construir(vector<Nodo> nodos, vector<Arista> aristas);

    int cantidadNodos() const;
    int cantidadAristas() const;
    bool existeNodeId(int nodeId) const;
    int obtenerIndiceDesdeNodeId(int nodeId) const;
    int obtenerNodeIdDesdeIndice(int indiceNodo) const;

    const vector<Nodo>& obtenerNodos() const;
    const vector<Arista>& obtenerAristas() const;
    const ListaAdyacencia& obtenerAdyacenciaDistancia() const;
    const ListaAdyacencia& obtenerAdyacenciaTiempo() const;
    const ListaAdyacencia& obtenerAdyacenciaNoDirigida() const;
};
