#include "conjunto_disjunto.h"

#include <numeric>

using namespace std;

ConjuntoDisjunto::ConjuntoDisjunto(int cant) {
    padre.resize(cant);
    iota(padre.begin(), padre.end(), 0);
}

int ConjuntoDisjunto::buscar(int nodo) {
    if (padre[nodo] == nodo) {
        return nodo;
    }
    padre[nodo] = buscar(padre[nodo]);
    return padre[nodo];
}

bool ConjuntoDisjunto::unir(int a, int b) {
    int padreA = buscar(a);
    int padreB = buscar(b);

    if (padreA == padreB) {
        return false;
    }

    padre[padreB] = padreA;
    return true;
}
