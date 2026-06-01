#pragma once

#include <vector>

using namespace std;

class ConjuntoDisjunto {
private:
    vector<int> padre;

public:
    ConjuntoDisjunto(int cant);

    int buscar(int nodo);
    bool unir(int a, int b);
};
