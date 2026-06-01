#pragma once

#include <string>

using namespace std;

struct Nodo {
    int id;
    double lat;
    double lon;
};

struct Arista {
    long long idOsm;
    int idOrigen;
    int idDestino;
    double distanciaMetros;
    string claseVia;
    int esUnSoloSentido;
    double velocidadMaxima;
};

struct EntradaAdyacencia {
    int hacia;
    double peso;
    int indiceArista;
};
