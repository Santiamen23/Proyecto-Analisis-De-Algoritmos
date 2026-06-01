#include "graph.h"

#include <algorithm>
#include <set>
#include <stdexcept>
#include <utility>

using namespace std;

namespace {
double convertirKmHams(double kilometrosPorHora) {
    return kilometrosPorHora / 3.6;
}
}

Grafo::Grafo() {
}

void Grafo::construir(vector<Nodo> nodos, vector<Arista> aristas) {
    this->nodos = nodos;
    this->aristas = aristas;
    this->indPorNodeId.clear();

    for (int i = 0; i < (int)this->nodos.size(); ++i) {
        this->indPorNodeId[this->nodos[i].id] = i;
    }

    this->adyDist.assign(this->nodos.size(), {});
    this->adyTiempo.assign(this->nodos.size(), {});
    this->adyNoDir.assign(this->nodos.size(), {});

    set<pair<int, int>> vistasDir;
    set<pair<int, int>> vistasNoDir;

    for (int indArista = 0; indArista < (int)this->aristas.size(); ++indArista) {
        Arista arista = this->aristas[indArista];
        auto iterOrigen = this->indPorNodeId.find(arista.idOrigen);
        auto iterDestino = this->indPorNodeId.find(arista.idDestino);
        if (iterOrigen == this->indPorNodeId.end() || iterDestino == this->indPorNodeId.end()) {
            continue;
        }

        int indOrigen = iterOrigen->second;
        int indDestino = iterDestino->second;
        double tiempoSeg = arista.distanciaMetros / convertirKmHams(arista.velocidadMaxima);

        if (!vistasDir.contains({indOrigen, indDestino})) {
            this->adyDist[indOrigen].push_back({indDestino, arista.distanciaMetros, indArista});
            this->adyTiempo[indOrigen].push_back({indDestino, tiempoSeg, indArista});
            vistasDir.insert({indOrigen, indDestino});
        }

        if (arista.esUnSoloSentido == 0 && !vistasDir.contains({indDestino, indOrigen})) {
            this->adyDist[indDestino].push_back({indOrigen, arista.distanciaMetros, indArista});
            this->adyTiempo[indDestino].push_back({indOrigen, tiempoSeg, indArista});
            vistasDir.insert({indDestino, indOrigen});
        }

        pair<int, int> clave = minmax(indOrigen, indDestino);
        if (!vistasNoDir.contains(clave)) {
            this->adyNoDir[indOrigen].push_back({indDestino, arista.distanciaMetros, indArista});
            this->adyNoDir[indDestino].push_back({indOrigen, arista.distanciaMetros, indArista});
            vistasNoDir.insert(clave);
        }
    }
}

int Grafo::cantidadNodos() {
    return (int)this->nodos.size();
}

int Grafo::cantidadAristas() {
    return (int)this->aristas.size();
}

bool Grafo::existeNodeId(int nodeId) {
    return this->indPorNodeId.contains(nodeId);
}

int Grafo::obtenerIndDesdeNodeId(int nodeId) {
    auto iter = this->indPorNodeId.find(nodeId);
    if (iter == this->indPorNodeId.end()) {
        throw out_of_range("Node ID not found in graph");
    }
    return iter->second;
}

int Grafo::obtenerNodeIdDesdeInd(int ind) {
    return this->nodos.at(ind).id;
}

vector<Nodo> Grafo::obtenerNodos() {
    return this->nodos;
}

vector<Arista> Grafo::obtenerAristas() {
    return this->aristas;
}

vector<vector<EntradaAdyacencia>> Grafo::obtenerAdyDist() {
    return this->adyDist;
}

vector<vector<EntradaAdyacencia>> Grafo::obtenerAdyTiempo() {
    return this->adyTiempo;
}

vector<vector<EntradaAdyacencia>> Grafo::obtenerAdyNoDir() {
    return this->adyNoDir;
}
