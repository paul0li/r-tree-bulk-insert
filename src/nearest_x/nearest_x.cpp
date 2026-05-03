#include "utils.hpp"
#include "nearest_x.hpp"
#include "structs.hpp"

using namespace std;

/// crea una entrada a partir de un punto (MBR de área 0, índice -1)
/// @param p Punto de origen.
/// @return Child con ese MBR degenerado e index -1.
static Child makeChildFromPoint(const Point& p) {
    Child e;
    e.mbr = makeRectFromPoint(p);
    e.index = -1;
    return e;
}

/// crea un nodo a partir de un rango de entradas (sin calcular MBR)
/// @param entries Vector fuente de Child.
/// @param l Índice inicial del rango (inclusive).
/// @param r Índice final exclusivo; el nodo tendrá k = r - l hijos copiados de entries[l .. r-1].
/// @return Nodo con hijos en ese rango.
static Node makeNodeFromEntries(const vector<Child>& entries, int l, int r) {
    Node node{};
    node.k = r - l;

    for (int i = 0; i < node.k; i++) {
        node.hijos[i] = entries[l + i];
    }

    return node;
}

/// convierte un vector de puntos en un vector de entradas (Child)
/// @param points Puntos de entrada.
/// @return Vector de Child en formato hoja (index -1 por punto).
static vector<Child> pointsToEntries(const vector<Point>& points) {
    vector<Child> entries;
    entries.reserve(points.size());

    for (const Point& p : points) {
        entries.push_back(makeChildFromPoint(p));
    }

    return entries;
}

/// construye un nivel del árbol a partir de las entradas actuales, devolviendo las entradas del nivel padre
/// @param current Entradas del nivel actual (se ordena por centro-X in-place y se agrupa en bloques de B).
/// @param tree Vector global de nodos; se hace push de cada nodo nuevo (su índice es la posición en el vector).
/// @return Lista de Child para el nivel superior: MBR de cada bloque + índice del nodo hijo correspondiente.
static vector<Child> buildNearestXLevel(vector<Child>& current, vector<Node>& tree) {
    sortEntriesByX(current);

    vector<Child> parentEntries;
    parentEntries.reserve((current.size() + B - 1) / B);

    for (int i = 0; i < (int)current.size(); i += B) {
        int l = i;
        int r = min(i + B, (int)current.size());

        Node node = makeNodeFromEntries(current, l, r);

        int nodeIndex = (int)tree.size();
        tree.push_back(node);

        Child parent;
        parent.mbr = calcMBR(node);
        parent.index = nodeIndex;

        parentEntries.push_back(parent);
    }

    return parentEntries;
}

/// Construye el árbol Nearest-X a partir de los puntos dados, devolviendo el vector de nodos (con raíz en tree[0])
/// @param points Puntos a indexar (orden del vector irrelevante).
/// @return Vector secuencial de nodos listo para serializar; tree[0] es la raíz.
vector<Node> buildNearestX(const vector<Point>& points) {
    vector<Node> tree;

    tree.push_back(Node{});

    vector<Child> current = pointsToEntries(points);

    while ((int)current.size() > B) {
        current = buildNearestXLevel(current, tree);
    }

    Node root{};
    root.k = (int)current.size();

    for (int i = 0; i < root.k; i++) {
        root.hijos[i] = current[i];
    }

    tree[0] = root;

    return tree;
}
