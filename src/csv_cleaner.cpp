#include "csv_cleaner.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace {
void intentarParsearNodo(const string& linea, Nodo& nodo) {
    stringstream flujo(linea);
    string token;

    getline(flujo, token, ',');
    nodo.nodeId = stoi(limpiarEspacios(token));
    getline(flujo, token, ',');
    nodo.latitud = stod(limpiarEspacios(token));
    getline(flujo, token, ',');
    nodo.longitud = stod(limpiarEspacios(token));
}

void intentarParsearArista(const string& linea, Arista& arista) {
    stringstream flujo(linea);
    string token;

    getline(flujo, token, ',');
    arista.osmWayId = stoll(limpiarEspacios(token));
    getline(flujo, token, ',');
    arista.nodeIdOrigen = stoi(limpiarEspacios(token));
    getline(flujo, token, ',');
    arista.nodeIdDestino = stoi(limpiarEspacios(token));
    getline(flujo, token, ',');
    arista.distanciaMetros = stod(limpiarEspacios(token));
    getline(flujo, token, ',');
    arista.claseVia = limpiarEspacios(token);
    getline(flujo, token, ',');
    arista.unSoloSentido = stoi(limpiarEspacios(token)) != 0;
    getline(flujo, token, ',');
    arista.velocidadMaximaKmh = parsearVelocidadMaxima(limpiarEspacios(token));
}
}

string limpiarEspacios(string texto) {
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    size_t fin = texto.find_last_not_of(" \t\r\n");
    return (inicio == string::npos) ? "" : texto.substr(inicio, fin - inicio + 1);
}

double parsearVelocidadMaxima(string texto) {
    if (texto.empty()) return 0.0;
    try {
        return stod(texto);
    }
    catch (...) {
        return 0.0;
    }
}

double obtenerVelocidadPorDefecto(string claseVia) {
    if (claseVia == "motorway") return 100.0;
    if (claseVia == "trunk") return 80.0;
    if (claseVia == "primary") return 60.0;
    if (claseVia == "secondary") return 50.0;
    if (claseVia == "tertiary") return 40.0;
    if (claseVia == "residential") return 30.0;
    if (claseVia == "path") return 10.0;
    if (claseVia == "primary_link") return 50.0;
    if (claseVia == "secondary_link") return 40.0;
    if (claseVia == "tertiary_link") return 30.0;
    return 20.0;
}

bool cargarNodos(const string& rutaArchivo, vector<Nodo>& nodos, EstadisticasCarga& estadisticas) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return false;
    }

    string linea;
    getline(archivo, linea);
    while (getline(archivo, linea)) {
        if (linea.empty()) {
            continue;
        }

        Nodo nodo;
        try {
            intentarParsearNodo(linea, nodo);
        }
        catch (...) {
            estadisticas.registrosDescartados++;
            continue;
        }

        nodos.push_back(nodo);
        estadisticas.registrosCargados++;
    }
    return true;
}

bool cargarAristas(const string& rutaArchivo, vector<Arista>& aristas, EstadisticasCarga& estadisticas) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return false;
    }

    string linea;
    getline(archivo, linea);
    while (getline(archivo, linea)) {
        if (linea.empty()) {
            continue;
        }

        Arista arista;
        try {
            intentarParsearArista(linea, arista);
        }
        catch (...) {
            estadisticas.registrosDescartados++;
            continue;
        }

        if (arista.distanciaMetros <= 0.0 || arista.nodeIdOrigen == arista.nodeIdDestino) {
            estadisticas.registrosDescartados++;
            continue;
        }

        if (arista.velocidadMaximaKmh <= 0.0) {
            arista.velocidadMaximaKmh = obtenerVelocidadPorDefecto(arista.claseVia);
        }

        aristas.push_back(arista);
        estadisticas.registrosCargados++;
    }
    return true;
}
