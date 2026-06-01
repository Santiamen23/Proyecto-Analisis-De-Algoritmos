#pragma once

#include <string>
#include <vector>

#include "models.h"

using namespace std;

struct EstadisticasCarga {
    int registrosCargados{};
    int registrosDescartados{};
};

string limpiarEspacios(string texto);
double parsearVelocidadMaxima(string texto);
double obtenerVelocidadPorDefecto(string claseVia);

bool cargarNodos(const string& rutaArchivo, vector<Nodo>& nodos, EstadisticasCarga& estadisticas);
bool cargarAristas(const string& rutaArchivo, vector<Arista>& aristas, EstadisticasCarga& estadisticas);
