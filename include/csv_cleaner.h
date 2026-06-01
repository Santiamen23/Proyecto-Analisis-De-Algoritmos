#pragma once

#include <string>
#include <vector>

#include "models.h"

using namespace std;

string limpiarEspacios(string texto);
double parsearVelocidadMaxima(string texto);
double obtenerVelocidadPorDefecto(string claseVia);

bool cargarNodos(string rutaArchivo, vector<Nodo>& nodos);
bool cargarAristas(string rutaArchivo, vector<Arista>& aristas);
