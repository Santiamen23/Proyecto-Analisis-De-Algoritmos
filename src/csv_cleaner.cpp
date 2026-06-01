#include "csv_cleaner.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

string limpiarEspacios(string texto) {
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    size_t fin = texto.find_last_not_of(" \t\r\n");
    return (inicio == string::npos) ? "" : texto.substr(inicio, fin - inicio + 1);
}

double parsearVelocidadMaxima(string texto)
{
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

bool cargarNodos(string rutaArchivo, vector<Nodo>& nodos) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return false;
    }

    string linea;
    getline(archivo, linea);
    int descartados = 0;
    while (getline(archivo, linea))
    {
        if (linea.empty()) continue;
        stringstream flujo(linea);
        string token;
        Nodo nodo;

        try {
            getline(flujo, token, ',');
            nodo.id = stoi(limpiarEspacios(token));
            getline(flujo, token, ',');
            nodo.lat = stod(limpiarEspacios(token));
            getline(flujo, token, ',');
            nodo.lon = stod(limpiarEspacios(token));
        }
        catch (...) {
            descartados++;
            continue;
        }
        nodos.push_back(nodo);
    }

    cout << "Nodos cargados: " << nodos.size() << endl;
    cout << "Nodos Descartados: " << descartados << endl;
    return true;
}

bool cargarAristas(string rutaArchivo, vector<Arista>& aristas) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return false;
    }

    string linea;
    getline(archivo, linea);
    int descartados = 0;
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;

        stringstream flujo(linea);
        string token;
        Arista arista;

        try {
            getline(flujo, token, ',');
            arista.idOsm = stoll(limpiarEspacios(token));
            getline(flujo, token, ',');
            arista.idOrigen = stoi(limpiarEspacios(token));
            getline(flujo, token, ',');
            arista.idDestino = stoi(limpiarEspacios(token));
            getline(flujo, token, ',');
            arista.distanciaMetros = stod(limpiarEspacios(token));
            getline(flujo, token, ',');
            arista.claseVia = limpiarEspacios(token);
            getline(flujo, token, ',');
            arista.esUnSoloSentido = stoi(limpiarEspacios(token));
            getline(flujo, token, ',');
            arista.velocidadMaxima = parsearVelocidadMaxima(limpiarEspacios(token));
        }
        catch (...) {
            descartados++;
            continue;
        }

        if (arista.distanciaMetros <= 0.0) {
            descartados++;
            continue;
        }

        if (arista.idOrigen == arista.idDestino) {
            descartados++;
            continue;
        }

        if (arista.velocidadMaxima <= 0.0) {
            arista.velocidadMaxima = obtenerVelocidadPorDefecto(arista.claseVia);
        }

        aristas.push_back(arista);
    }

    cerr << "Aristas cargadas: " << aristas.size() << endl;
    cerr << "Descartados: " << descartados << endl;
    return true;
}
