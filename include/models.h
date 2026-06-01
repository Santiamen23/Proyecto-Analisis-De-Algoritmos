#pragma once

#include <string>

using namespace std;

struct Nodo {
    int nodeId;
    double latitud;
    double longitud;
};

struct Arista {
    long long osmWayId;
    int nodeIdOrigen;
    int nodeIdDestino;
    double distanciaMetros;
    string claseVia;
    bool unSoloSentido;
    double velocidadMaximaKmh;
};

struct AristaAdyacente {
    int indiceDestino;
    double costo;
    int indiceAristaOriginal;
};
