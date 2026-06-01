#include "graph.h"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <unordered_set>
#include <utility>

using namespace std;

namespace {
double convertirKmHams(double kilometrosPorHora) {
    return kilometrosPorHora / 3.6;
}

uint64_t codificarPar(int a, int b) {
    return (static_cast<uint64_t>(static_cast<uint32_t>(a)) << 32) |
           static_cast<uint32_t>(b);
}
}

void Grafo::reconstruirIndiceNodos() {
    this->indicePorNodeId.clear();
    for (int indiceNodo = 0; indiceNodo < static_cast<int>(this->nodos.size()); ++indiceNodo) {
        this->indicePorNodeId[this->nodos[indiceNodo].nodeId] = indiceNodo;
    }
}

void Grafo::inicializarListasAdyacencia() {
    this->adyacenciaDistancia.assign(this->nodos.size(), {});
    this->adyacenciaTiempo.assign(this->nodos.size(), {});
    this->adyacenciaNoDirigida.assign(this->nodos.size(), {});
}

void Grafo::registrarAristaDirigida(int indiceOrigen, int indiceDestino, double distanciaMetros, double tiempoSegundos, int indiceArista) {
    this->adyacenciaDistancia[indiceOrigen].push_back({indiceDestino, distanciaMetros, indiceArista});
    this->adyacenciaTiempo[indiceOrigen].push_back({indiceDestino, tiempoSegundos, indiceArista});
}

void Grafo::registrarAristaNoDirigida(int indiceOrigen, int indiceDestino, double distanciaMetros, int indiceArista) {
    this->adyacenciaNoDirigida[indiceOrigen].push_back({indiceDestino, distanciaMetros, indiceArista});
    this->adyacenciaNoDirigida[indiceDestino].push_back({indiceOrigen, distanciaMetros, indiceArista});
}

void Grafo::construir(vector<Nodo> nodos, vector<Arista> aristas) {
    this->nodos = move(nodos);
    this->aristas = move(aristas);
    reconstruirIndiceNodos();
    inicializarListasAdyacencia();

    unordered_set<uint64_t> vistasDir;
    unordered_set<uint64_t> vistasNoDir;
    vistasDir.reserve(this->aristas.size() * 2);
    vistasNoDir.reserve(this->aristas.size());

    for (int indiceArista = 0; indiceArista < static_cast<int>(this->aristas.size()); ++indiceArista) {
        const Arista& arista = this->aristas[indiceArista];
        auto iterOrigen = this->indicePorNodeId.find(arista.nodeIdOrigen);
        auto iterDestino = this->indicePorNodeId.find(arista.nodeIdDestino);
        if (iterOrigen == this->indicePorNodeId.end() || iterDestino == this->indicePorNodeId.end()) {
            continue;
        }

        const int indiceOrigen = iterOrigen->second;
        const int indiceDestino = iterDestino->second;
        const double tiempoSegundos = arista.distanciaMetros / convertirKmHams(arista.velocidadMaximaKmh);

        const uint64_t claveDir = codificarPar(indiceOrigen, indiceDestino);
        if (!vistasDir.contains(claveDir)) {
            registrarAristaDirigida(indiceOrigen, indiceDestino, arista.distanciaMetros, tiempoSegundos, indiceArista);
            vistasDir.insert(claveDir);
        }

        const uint64_t claveDirInversa = codificarPar(indiceDestino, indiceOrigen);
        if (!arista.unSoloSentido && !vistasDir.contains(claveDirInversa)) {
            registrarAristaDirigida(indiceDestino, indiceOrigen, arista.distanciaMetros, tiempoSegundos, indiceArista);
            vistasDir.insert(claveDirInversa);
        }

        const auto [menor, mayor] = minmax(indiceOrigen, indiceDestino);
        const uint64_t claveNoDir = codificarPar(menor, mayor);
        if (!vistasNoDir.contains(claveNoDir)) {
            registrarAristaNoDirigida(indiceOrigen, indiceDestino, arista.distanciaMetros, indiceArista);
            vistasNoDir.insert(claveNoDir);
        }
    }
}

int Grafo::cantidadNodos() const {
    return static_cast<int>(this->nodos.size());
}

int Grafo::cantidadAristas() const {
    return static_cast<int>(this->aristas.size());
}

bool Grafo::existeNodeId(int nodeId) const {
    return this->indicePorNodeId.contains(nodeId);
}

int Grafo::obtenerIndiceDesdeNodeId(int nodeId) const {
    auto iter = this->indicePorNodeId.find(nodeId);
    if (iter == this->indicePorNodeId.end()) {
        throw out_of_range("El node_id no existe en el grafo");
    }
    return iter->second;
}

int Grafo::obtenerNodeIdDesdeIndice(int indiceNodo) const {
    return this->nodos.at(indiceNodo).nodeId;
}

const vector<Nodo>& Grafo::obtenerNodos() const {
    return this->nodos;
}

const vector<Arista>& Grafo::obtenerAristas() const {
    return this->aristas;
}

const ListaAdyacencia& Grafo::obtenerAdyacenciaDistancia() const {
    return this->adyacenciaDistancia;
}

const ListaAdyacencia& Grafo::obtenerAdyacenciaTiempo() const {
    return this->adyacenciaTiempo;
}

const ListaAdyacencia& Grafo::obtenerAdyacenciaNoDirigida() const {
    return this->adyacenciaNoDirigida;
}
