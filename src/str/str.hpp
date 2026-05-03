#pragma once
#include <vector>
#include "structs.hpp"

using namespace std;

/// R-tree por STR (orden global por centro-X en franjas, luego Y dentro de cada franja, grupos de B).
/// @param points Puntos a indexar (orden irrelevante).
/// @return Nodos en orden serial; tree[0] es raíz; index en internos apunta a posición en el vector.
vector<Node> buildSTR(const vector<Point>& points);
