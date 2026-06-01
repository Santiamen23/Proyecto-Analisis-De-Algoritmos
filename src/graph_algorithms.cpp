#include "graph_algorithms.h"
#include "conjunto_disjunto.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <set>
#include <stdexcept>
#include <tuple>

using namespace std;

namespace {
double infinito() {
    return numeric_limits<double>::infinity();
}
}

AlgoritmosGrafo::AlgoritmosGrafo(Grafo* grafo) {
    this->grafo = grafo;
}

AlgoritmosGrafo::ResultadoDijkstra AlgoritmosGrafo::dijkstra(int indOrigen, vector<vector<EntradaAdyacencia>> adyacencia) {
    ResultadoDijkstra resultado;
    resultado.distancias.assign(this->grafo->cantidadNodos(), infinito());
    resultado.padre.assign(this->grafo->cantidadNodos(), -1);
    resultado.indiceAristaPadre.assign(this->grafo->cantidadNodos(), -1);

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> cola;
    resultado.distancias[indOrigen] = 0.0;
    cola.push({0.0, indOrigen});

    while (!cola.empty()) {
        auto actual = cola.top();
        cola.pop();

        double distAct = actual.first;
        int nodo = actual.second;

        if (distAct > resultado.distancias[nodo]) {
            continue;
        }

        for (EntradaAdyacencia sig : adyacencia[nodo]) {
            double nuevaDist = distAct + sig.peso;
            if (nuevaDist < resultado.distancias[sig.hacia]) {
                resultado.distancias[sig.hacia] = nuevaDist;
                resultado.padre[sig.hacia] = nodo;
                resultado.indiceAristaPadre[sig.hacia] = sig.indiceArista;
                cola.push({nuevaDist, sig.hacia});
            }
        }
    }

    return resultado;
}

double AlgoritmosGrafo::calcularTiempoAristaSegundos(Arista arista) {
    return arista.distanciaMetros / (arista.velocidadMaxima / 3.6);
}

DatosRuta AlgoritmosGrafo::construirDatosRuta(int indOrigen, int indDestino, ResultadoDijkstra resultado) {
    if (!isfinite(resultado.distancias[indDestino])) {
        throw runtime_error("No existe ruta entre los nodos seleccionados");
    }

    vector<int> ruta;
    for (int act = indDestino; act != -1; act = resultado.padre[act]) {
        ruta.push_back(act);
    }
    reverse(ruta.begin(), ruta.end());

    DatosRuta datosRuta;
    datosRuta.distanciaMetros = 0.0;
    datosRuta.tiempoSegundos = 0.0;

    for (int ind : ruta) {
        datosRuta.recorridoNodeIds.push_back(this->grafo->obtenerNodeIdDesdeInd(ind));
    }

    if (!ruta.empty() && ruta.front() != indOrigen) {
        throw runtime_error("La ruta no coincide con el origen");
    }

    vector<Arista> aristas = this->grafo->obtenerAristas();
    for (int i = 1; i < (int)ruta.size(); ++i) {
        int indNodo = ruta[i];
        Arista arista = aristas[resultado.indiceAristaPadre[indNodo]];
        datosRuta.distanciaMetros += arista.distanciaMetros;
        datosRuta.tiempoSegundos += calcularTiempoAristaSegundos(arista);
    }

    return datosRuta;
}

ResultadoAlcance AlgoritmosGrafo::calcularAlcanceVehicular(int nodeIdOrigen, double limiteMetros) {
    int indOrigen = this->grafo->obtenerIndDesdeNodeId(nodeIdOrigen);
    ResultadoDijkstra resultado = dijkstra(indOrigen, this->grafo->obtenerAdyDist());

    int cantidadAlcanzable = 0;
    for (double distancia : resultado.distancias) {
        if (distancia <= limiteMetros) {
            cantidadAlcanzable++;
        }
    }

    ResultadoAlcance salida;
    salida.nodeIdOrigen = nodeIdOrigen;
    salida.cantidadAlcanzable = cantidadAlcanzable;
    salida.porcentaje = this->grafo->cantidadNodos() == 0 ? 0.0 : 100.0 * cantidadAlcanzable / this->grafo->cantidadNodos();
    return salida;
}

ResultadoComponentes AlgoritmosGrafo::calcularComponentesDebilmenteConexas() {
    ResultadoComponentes resultado;
    resultado.componentePorNodo.assign(this->grafo->cantidadNodos(), -1);

    int compAct = 0;
    int tamGig = 0;
    int idGig = -1;
    queue<int> cola;
    vector<vector<EntradaAdyacencia>> ady = this->grafo->obtenerAdyNoDir();

    for (int ini = 0; ini < this->grafo->cantidadNodos(); ++ini) {
        if (resultado.componentePorNodo[ini] != -1) {
            continue;
        }

        resultado.componentes.push_back({});
        resultado.componentePorNodo[ini] = compAct;
        cola.push(ini);

        while (!cola.empty()) {
            int nodo = cola.front();
            cola.pop();
            resultado.componentes.back().push_back(nodo);

            for (EntradaAdyacencia sig : ady[nodo]) {
                if (resultado.componentePorNodo[sig.hacia] == -1) {
                    resultado.componentePorNodo[sig.hacia] = compAct;
                    cola.push(sig.hacia);
                }
            }
        }

        if ((int)resultado.componentes.back().size() > tamGig) {
            tamGig = (int)resultado.componentes.back().size();
            idGig = compAct;
        }

        compAct++;
    }

    resultado.cantidadComponentes = compAct;
    resultado.tamanoComponenteGigante = tamGig;
    resultado.idComponenteGigante = idGig;
    return resultado;
}

ResultadoDiametro AlgoritmosGrafo::calcularDiametroVial() {
    ResultadoComponentes componentes = calcularComponentesDebilmenteConexas();
    if (componentes.idComponenteGigante == -1) {
        throw runtime_error("El grafo no tiene componentes");
    }

    ResultadoDiametro mejor;
    mejor.nodeIdOrigen = this->grafo->obtenerNodeIdDesdeInd(componentes.componentes[componentes.idComponenteGigante].front());
    mejor.nodeIdDestino = this->grafo->obtenerNodeIdDesdeInd(componentes.componentes[componentes.idComponenteGigante].front());
    mejor.distanciaMetros = 0.0;

    vector<int> nodosGig = componentes.componentes[componentes.idComponenteGigante];
    vector<vector<EntradaAdyacencia>> ady = this->grafo->obtenerAdyDist();
    for (int indOrigen : nodosGig) {
        ResultadoDijkstra resultado = dijkstra(indOrigen, ady);
        for (int indDestino : nodosGig) {
            if (resultado.distancias[indDestino] < infinito() && resultado.distancias[indDestino] > mejor.distanciaMetros) {
                mejor.nodeIdOrigen = this->grafo->obtenerNodeIdDesdeInd(indOrigen);
                mejor.nodeIdDestino = this->grafo->obtenerNodeIdDesdeInd(indDestino);
                mejor.distanciaMetros = resultado.distancias[indDestino];
            }
        }
    }

    return mejor;
}

ResultadoMst AlgoritmosGrafo::calcularRedEmergenciaMinima() {
    ResultadoComponentes componentes = calcularComponentesDebilmenteConexas();
    if (componentes.idComponenteGigante == -1) {
        throw runtime_error("El grafo no tiene componentes");
    }

    vector<int> nodosGig = componentes.componentes[componentes.idComponenteGigante];
    vector<int> indLocal(this->grafo->cantidadNodos(), -1);
    for (int i = 0; i < (int)nodosGig.size(); ++i) {
        indLocal[nodosGig[i]] = i;
    }

    vector<tuple<double, int, int>> aristasCand;
    set<pair<int, int>> vistas;
    vector<vector<EntradaAdyacencia>> ady = this->grafo->obtenerAdyNoDir();

    for (int indOrigen : nodosGig) {
        for (EntradaAdyacencia sig : ady[indOrigen]) {
            if (indLocal[sig.hacia] == -1) {
                continue;
            }
            int a = indLocal[indOrigen];
            int b = indLocal[sig.hacia];
            int menor = min(a, b);
            int mayor = max(a, b);
            if (!vistas.contains({menor, mayor})) {
                vistas.insert({menor, mayor});
                aristasCand.push_back({sig.peso, menor, mayor});
            }
        }
    }

    sort(aristasCand.begin(), aristasCand.end());

    ConjuntoDisjunto cd((int)nodosGig.size());
    double distTotal = 0.0;
    int aristasUsadas = 0;

    for (auto dato : aristasCand) {
        double peso = get<0>(dato);
        int a = get<1>(dato);
        int b = get<2>(dato);
        if (cd.unir(a, b)) {
            distTotal += peso;
            aristasUsadas++;
        }
    }

    ResultadoMst resultado;
    resultado.tamanoComponenteGigante = (int)nodosGig.size();
    resultado.aristasUsadas = aristasUsadas;
    resultado.distanciaTotalMetros = distTotal;
    return resultado;
}

ResultadoRutas AlgoritmosGrafo::compararRutas(int nodeIdOrigen, int nodeIdDestino) {
    int indOrigen = this->grafo->obtenerIndDesdeNodeId(nodeIdOrigen);
    int indDestino = this->grafo->obtenerIndDesdeNodeId(nodeIdDestino);

    ResultadoDijkstra porDist = dijkstra(indOrigen, this->grafo->obtenerAdyDist());
    ResultadoDijkstra porTiempo = dijkstra(indOrigen, this->grafo->obtenerAdyTiempo());

    ResultadoRutas resultado;
    resultado.nodeIdOrigen = nodeIdOrigen;
    resultado.nodeIdDestino = nodeIdDestino;
    resultado.rutaPorDistancia = construirDatosRuta(indOrigen, indDestino, porDist);
    resultado.rutaPorTiempo = construirDatosRuta(indOrigen, indDestino, porTiempo);
    return resultado;
}

