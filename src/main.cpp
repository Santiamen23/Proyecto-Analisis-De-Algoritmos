#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "csv_cleaner.h"
#include "graph.h"
#include "graph_algorithms.h"

using namespace std;
using Clock = chrono::high_resolution_clock;
using TimePoint = chrono::time_point<Clock>;

double tiempoTranscurridoMs(TimePoint inicio, TimePoint fin) {
    return chrono::duration<double, milli>(fin - inicio).count();
}

void mostrarVistaPreviaRuta(const vector<int>& recorridoNodeIds) {
    cout << "Nodos en ruta: " << recorridoNodeIds.size() << '\n';
    if (recorridoNodeIds.empty()) {
        return;
    }

    cout << "Vista previa: ";
    const int cantidadVistaPrevia = min<int>(5, static_cast<int>(recorridoNodeIds.size()));
    for (int i = 0; i < cantidadVistaPrevia; ++i) {
        if (i > 0) {
            cout << " -> ";
        }
        cout << recorridoNodeIds[i];
    }

    if (static_cast<int>(recorridoNodeIds.size()) > cantidadVistaPrevia) {
        cout << " -> ... -> " << recorridoNodeIds.back();
    }
    cout << '\n';
}

void mostrarEstadisticasCarga(const string& etiqueta, const EstadisticasCarga& estadisticas) {
    cerr << etiqueta << " cargados: " << estadisticas.registrosCargados << '\n';
    cerr << etiqueta << " descartados: " << estadisticas.registrosDescartados << '\n';
}

int leerNodeId(const string& mensaje) {
    int nodeId = 0;
    cout << mensaje;
    cin >> nodeId;
    return nodeId;
}

void ejecutarAnalisisAlcance(const AlgoritmosGrafo& algoritmos) {
    const int nodeIdOrigen = leerNodeId("Ingresa node_id origen: ");
    const TimePoint tiempoInicio = Clock::now();
    const ResultadoAlcance resultado = algoritmos.calcularAlcanceVehicular(nodeIdOrigen);
    const TimePoint tiempoFin = Clock::now();

    cout << fixed << setprecision(2);
    cout << "Origen: " << resultado.nodeIdOrigen << '\n';
    cout << "Nodos alcanzables en <= 5 km: " << resultado.cantidadAlcanzable << '\n';
    cout << "Porcentaje del grafo: " << resultado.porcentaje << "%\n";
    cerr << "Tiempo alcance vehicular: " << tiempoTranscurridoMs(tiempoInicio, tiempoFin) << " ms" << endl;
}

void ejecutarAnalisisComponentes(const AlgoritmosGrafo& algoritmos) {
    const TimePoint tiempoInicio = Clock::now();
    const ResultadoComponentes resultado = algoritmos.calcularComponentesDebilmenteConexas();
    const TimePoint tiempoFin = Clock::now();

    cout << "Numero total de islas/componentes: " << resultado.cantidadComponentes << '\n';
    cout << "Tamano de la componente principal: " << resultado.tamanoComponenteGigante << '\n';
    cerr << "Tiempo componentes: " << tiempoTranscurridoMs(tiempoInicio, tiempoFin) << " ms" << endl;
}

void ejecutarAnalisisDiametro(const AlgoritmosGrafo& algoritmos) {
    const TimePoint tiempoInicio = Clock::now();
    const ResultadoDiametro resultado = algoritmos.calcularDiametroVial();
    const TimePoint tiempoFin = Clock::now();

    cout << fixed << setprecision(2);
    cout << "Par mas alejado en la componente gigante:\n";
    cout << "Origen: " << resultado.nodeIdOrigen << '\n';
    cout << "Destino: " << resultado.nodeIdDestino << '\n';
    cout << "Distancia minima entre ambos: " << resultado.distanciaMetros << " m\n";
    cerr << "Tiempo diametro vial: " << tiempoTranscurridoMs(tiempoInicio, tiempoFin) << " ms" << endl;
}

void ejecutarAnalisisMst(const AlgoritmosGrafo& algoritmos) {
    const TimePoint tiempoInicio = Clock::now();
    const ResultadoMst resultado = algoritmos.calcularRedEmergenciaMinima();
    const TimePoint tiempoFin = Clock::now();

    cout << fixed << setprecision(2);
    cout << "Componente gigante: " << resultado.tamanoComponenteGigante << " nodos\n";
    cout << "Aristas usadas en el MST: " << resultado.aristasUsadas << '\n';
    cout << "Distancia total cubierta: " << resultado.distanciaTotalMetros / 1000.0 << " km\n";
    cerr << "Tiempo MST: " << tiempoTranscurridoMs(tiempoInicio, tiempoFin) << " ms" << endl;
}

void ejecutarComparacionRutas(const AlgoritmosGrafo& algoritmos) {
    const int nodeIdOrigen = leerNodeId("Ingresa node_id origen: ");
    const int nodeIdDestino = leerNodeId("Ingresa node_id destino: ");

    const TimePoint tiempoInicio = Clock::now();
    const ResultadoRutas resultado = algoritmos.compararRutas(nodeIdOrigen, nodeIdDestino);
    const TimePoint tiempoFin = Clock::now();

    cout << fixed << setprecision(2);
    cout << "Ruta minima por DISTANCIA\n";
    cout << "Distancia: " << resultado.rutaPorDistancia.distanciaMetros / 1000.0 << " km\n";
    cout << "Tiempo estimado: " << resultado.rutaPorDistancia.tiempoSegundos / 60.0 << " min\n";
    mostrarVistaPreviaRuta(resultado.rutaPorDistancia.recorridoNodeIds);
    cout << '\n';

    cout << "Ruta minima por TIEMPO\n";
    cout << "Tiempo: " << resultado.rutaPorTiempo.tiempoSegundos / 60.0 << " min\n";
    cout << "Distancia recorrida: " << resultado.rutaPorTiempo.distanciaMetros / 1000.0 << " km\n";
    mostrarVistaPreviaRuta(resultado.rutaPorTiempo.recorridoNodeIds);
    cerr << "Tiempo comparacion de rutas: " << tiempoTranscurridoMs(tiempoInicio, tiempoFin) << " ms" << endl;
}

int main() {
    const string rutaNodos = "C:\\Users\\Santiago\\Desktop\\Universidad\\V SEMESTRE\\ANALISIS\\nodes.csv";
    const string rutaAristas = "C:\\Users\\Santiago\\Desktop\\Universidad\\V SEMESTRE\\ANALISIS\\edges.csv";

    vector<Nodo> nodos;
    vector<Arista> aristas;
    EstadisticasCarga estadisticasNodos;
    EstadisticasCarga estadisticasAristas;

    cerr << "===== CARGANDO DATOS =====" << endl;
    const TimePoint tiempoInicioTotal = Clock::now();
    const TimePoint tiempoInicioCarga = Clock::now();
    if (!cargarNodos(rutaNodos, nodos, estadisticasNodos)) {
        return 1;
    }
    if (!cargarAristas(rutaAristas, aristas, estadisticasAristas)) {
        return 1;
    }
    const TimePoint tiempoFinCarga = Clock::now();
    mostrarEstadisticasCarga("Nodos", estadisticasNodos);
    mostrarEstadisticasCarga("Aristas", estadisticasAristas);
    cerr << "Tiempo de carga: " << fixed << setprecision(2) << tiempoTranscurridoMs(tiempoInicioCarga, tiempoFinCarga) << " ms" << endl;

    cerr << "===== CONSTRUYENDO GRAFO =====" << endl;
    const TimePoint tiempoInicioGrafo = Clock::now();
    Grafo grafo;
    grafo.construir(nodos, aristas);
    const TimePoint tiempoFinGrafo = Clock::now();
    cerr << "Nodos: " << grafo.cantidadNodos() << endl;
    cerr << "Aristas cargadas: " << grafo.cantidadAristas() << endl;
    cerr << "Tiempo de construccion: " << tiempoTranscurridoMs(tiempoInicioGrafo, tiempoFinGrafo) << " ms" << endl;

    AlgoritmosGrafo algoritmos(&grafo);

    int opcion = 0;
    cout << "\n----- MENU DE ANALISIS -----\n";
    cout << "1. Alcance vehicular (<= 5 km)\n";
    cout << "2. Islas viales\n";
    cout << "3. Diametro vial\n";
    cout << "4. Red de emergencia minima (MST)\n";
    cout << "5. Comparar ruta por distancia vs tiempo\n";
    cout << "Ingresa una opcion: ";
    cin >> opcion;

    try {
        switch (opcion) {
        case 1:
            ejecutarAnalisisAlcance(algoritmos);
            break;
        case 2:
            ejecutarAnalisisComponentes(algoritmos);
            break;
        case 3:
            ejecutarAnalisisDiametro(algoritmos);
            break;
        case 4:
            ejecutarAnalisisMst(algoritmos);
            break;
        case 5:
            ejecutarComparacionRutas(algoritmos);
            break;
        default:
            cerr << "Opcion invalida" << endl;
            return 1;
        }
    }
    catch (const exception& excepcion) {
        cerr << "Error: " << excepcion.what() << endl;
        return 1;
    }

    const TimePoint tiempoFinTotal = Clock::now();
    cerr << "===== ANALISIS COMPLETADO =====" << endl;
    cerr << "Tiempo total: " << tiempoTranscurridoMs(tiempoInicioTotal, tiempoFinTotal) << " ms" << endl;
    return 0;
}
