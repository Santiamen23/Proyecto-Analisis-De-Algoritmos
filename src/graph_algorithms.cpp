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

AlgoritmosGrafo::AlgoritmosGrafo(const Grafo* grafo) {
    this->grafo = grafo;
}

AlgoritmosGrafo::ResultadoDijkstra AlgoritmosGrafo::dijkstra(int indiceOrigen, const ListaAdyacencia& adyacencia) const {
    ResultadoDijkstra resultado;
    resultado.distancias.assign(this->grafo->cantidadNodos(), infinito());
    resultado.padre.assign(this->grafo->cantidadNodos(), -1);
    resultado.indiceAristaPadre.assign(this->grafo->cantidadNodos(), -1);

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> cola;
    resultado.distancias[indiceOrigen] = 0.0;
    cola.push({0.0, indiceOrigen});

    while (!cola.empty()) {
        auto actual = cola.top();
        cola.pop();

        double distAct = actual.first;
        int nodo = actual.second;

        if (distAct > resultado.distancias[nodo]) {
            continue;
        }

        for (const AristaAdyacente& siguiente : adyacencia[nodo]) {
            double nuevaDist = distAct + siguiente.costo;
            if (nuevaDist < resultado.distancias[siguiente.indiceDestino]) {
                resultado.distancias[siguiente.indiceDestino] = nuevaDist;
                resultado.padre[siguiente.indiceDestino] = nodo;
                resultado.indiceAristaPadre[siguiente.indiceDestino] = siguiente.indiceAristaOriginal;
                cola.push({nuevaDist, siguiente.indiceDestino});
            }
        }
    }

    return resultado;
}

double AlgoritmosGrafo::calcularTiempoAristaSegundos(const Arista& arista) const {
    return arista.distanciaMetros / (arista.velocidadMaximaKmh / 3.6);
}

DatosRuta AlgoritmosGrafo::construirDatosRuta(int indiceOrigen, int indiceDestino, const ResultadoDijkstra& resultado) const {
    if (!isfinite(resultado.distancias[indiceDestino])) {
        throw runtime_error("No existe ruta entre los nodos seleccionados");
    }

    vector<int> ruta;
    for (int act = indiceDestino; act != -1; act = resultado.padre[act]) {
        ruta.push_back(act);
    }
    reverse(ruta.begin(), ruta.end());

    DatosRuta datosRuta;
    datosRuta.distanciaMetros = 0.0;
    datosRuta.tiempoSegundos = 0.0;

    for (int indiceNodo : ruta) {
        datosRuta.recorridoNodeIds.push_back(this->grafo->obtenerNodeIdDesdeIndice(indiceNodo));
    }

    if (!ruta.empty() && ruta.front() != indiceOrigen) {
        throw runtime_error("La ruta no coincide con el origen");
    }

    const vector<Arista>& aristas = this->grafo->obtenerAristas();
    for (int i = 1; i < static_cast<int>(ruta.size()); ++i) {
        int indiceNodo = ruta[i];
        const Arista& arista = aristas[resultado.indiceAristaPadre[indiceNodo]];
        datosRuta.distanciaMetros += arista.distanciaMetros;
        datosRuta.tiempoSegundos += calcularTiempoAristaSegundos(arista);
    }

    return datosRuta;
}

ResultadoAlcance AlgoritmosGrafo::calcularAlcanceVehicular(int nodeIdOrigen, double limiteMetros) const {
    int indiceOrigen = this->grafo->obtenerIndiceDesdeNodeId(nodeIdOrigen);
    ResultadoDijkstra resultado = dijkstra(indiceOrigen, this->grafo->obtenerAdyacenciaDistancia());

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

ResultadoComponentes AlgoritmosGrafo::calcularComponentesDebilmenteConexas() const {
    ResultadoComponentes resultado;
    resultado.componentePorNodo.assign(this->grafo->cantidadNodos(), -1);

    int compAct = 0;
    int tamGig = 0;
    int idGig = -1;
    queue<int> cola;
    const ListaAdyacencia& ady = this->grafo->obtenerAdyacenciaNoDirigida();

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

            for (const AristaAdyacente& siguiente : ady[nodo]) {
                if (resultado.componentePorNodo[siguiente.indiceDestino] == -1) {
                    resultado.componentePorNodo[siguiente.indiceDestino] = compAct;
                    cola.push(siguiente.indiceDestino);
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

ResultadoDiametro AlgoritmosGrafo::calcularDiametroVial() const {
    ResultadoComponentes componentes = calcularComponentesDebilmenteConexas();
    if (componentes.idComponenteGigante == -1) {
        throw runtime_error("El grafo no tiene componentes");
    }

    ResultadoDiametro mejor;
    mejor.nodeIdOrigen = this->grafo->obtenerNodeIdDesdeIndice(componentes.componentes[componentes.idComponenteGigante].front());
    mejor.nodeIdDestino = this->grafo->obtenerNodeIdDesdeIndice(componentes.componentes[componentes.idComponenteGigante].front());
    mejor.distanciaMetros = 0.0;

    const vector<int>& nodosGig = componentes.componentes[componentes.idComponenteGigante];
    const ListaAdyacencia& ady = this->grafo->obtenerAdyacenciaDistancia();
    for (int indiceOrigen : nodosGig) {
        ResultadoDijkstra resultado = dijkstra(indiceOrigen, ady);
        for (int indiceDestino : nodosGig) {
            if (resultado.distancias[indiceDestino] < infinito() && resultado.distancias[indiceDestino] > mejor.distanciaMetros) {
                mejor.nodeIdOrigen = this->grafo->obtenerNodeIdDesdeIndice(indiceOrigen);
                mejor.nodeIdDestino = this->grafo->obtenerNodeIdDesdeIndice(indiceDestino);
                mejor.distanciaMetros = resultado.distancias[indiceDestino];
            }
        }
    }

    return mejor;
}

ResultadoMst AlgoritmosGrafo::calcularRedEmergenciaMinima() const {
    ResultadoComponentes componentes = calcularComponentesDebilmenteConexas();
    if (componentes.idComponenteGigante == -1) {
        throw runtime_error("El grafo no tiene componentes");
    }

    const vector<int>& nodosGig = componentes.componentes[componentes.idComponenteGigante];
    vector<int> indLocal(this->grafo->cantidadNodos(), -1);
    for (int i = 0; i < (int)nodosGig.size(); ++i) {
        indLocal[nodosGig[i]] = i;
    }

    vector<tuple<double, int, int>> aristasCand;
    aristasCand.reserve(this->grafo->cantidadAristas());

    set<pair<int, int>> vistas;

    for (const Arista& arista : this->grafo->obtenerAristas()) {
        if (!this->grafo->existeNodeId(arista.nodeIdOrigen) || !this->grafo->existeNodeId(arista.nodeIdDestino)) {
            continue;
        }

        const int indiceOrigen = this->grafo->obtenerIndiceDesdeNodeId(arista.nodeIdOrigen);
        const int indiceDestino = this->grafo->obtenerIndiceDesdeNodeId(arista.nodeIdDestino);

        if (indLocal[indiceOrigen] == -1 || indLocal[indiceDestino] == -1) {
            continue;
        }

        const int a = indLocal[indiceOrigen];
        const int b = indLocal[indiceDestino];
        const int menor = min(a, b);
        const int mayor = max(a, b);
        const pair<int, int> clave = {menor, mayor};
        if (vistas.contains(clave)) {
            continue;
        }

        vistas.insert(clave);
        aristasCand.push_back({arista.distanciaMetros, menor, mayor});
    }

    sort(aristasCand.begin(), aristasCand.end());

    ConjuntoDisjunto cd((int)nodosGig.size());
    double distTotal = 0.0;
    int aristasUsadas = 0;

    for (const auto& dato : aristasCand) {
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

ResultadoRutas AlgoritmosGrafo::compararRutas(int nodeIdOrigen, int nodeIdDestino) const {
    int indiceOrigen = this->grafo->obtenerIndiceDesdeNodeId(nodeIdOrigen);
    int indiceDestino = this->grafo->obtenerIndiceDesdeNodeId(nodeIdDestino);

    ResultadoDijkstra porDist = dijkstra(indiceOrigen, this->grafo->obtenerAdyacenciaDistancia());
    ResultadoDijkstra porTiempo = dijkstra(indiceOrigen, this->grafo->obtenerAdyacenciaTiempo());

    ResultadoRutas resultado;
    resultado.nodeIdOrigen = nodeIdOrigen;
    resultado.nodeIdDestino = nodeIdDestino;
    resultado.rutaPorDistancia = construirDatosRuta(indiceOrigen, indiceDestino, porDist);
    resultado.rutaPorTiempo = construirDatosRuta(indiceOrigen, indiceDestino, porTiempo);
    return resultado;
}
