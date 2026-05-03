#pragma once
#include "structs.hpp"
#include <vector>
using namespace std;

/// R-tree por Nearest-X (orden por centro-X, grupos de B, raíz al final en tree[0]).
/// @param points Puntos a indexar (orden irrelevante).
/// @return Nodos en orden serial; tree[0] es raíz; index en internos apunta a posición en el vector.
vector<Node> buildNearestX(const vector<Point>& points);
