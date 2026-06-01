#pragma once

#include <unordered_map>
#include <vector>

#include "models.h"

using namespace std;

class Grafo {
private:
    vector<Nodo> nodos;
    vector<Arista> aristas;
    vector<vector<EntradaAdyacencia>> adyDist;
    vector<vector<EntradaAdyacencia>> adyTiempo;
    vector<vector<EntradaAdyacencia>> adyNoDir;
    unordered_map<int, int> indPorNodeId;

public:
    Grafo();

    void construir(vector<Nodo> nodos, vector<Arista> aristas);

    int cantidadNodos();
    int cantidadAristas();
    bool existeNodeId(int nodeId);
    int obtenerIndDesdeNodeId(int nodeId);
    int obtenerNodeIdDesdeInd(int ind);

    vector<Nodo> obtenerNodos();
    vector<Arista> obtenerAristas();
    vector<vector<EntradaAdyacencia>> obtenerAdyDist();
    vector<vector<EntradaAdyacencia>> obtenerAdyTiempo();
    vector<vector<EntradaAdyacencia>> obtenerAdyNoDir();
};
