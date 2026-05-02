#include "utils.hpp"
#include "nearest_x.hpp"
#include "structs.hpp"

using namespace std;

// crea una entrada a partir de un punto (MBR de área 0, índice -1)
static Child makeChildFromPoint(const Point& p) {
    Child e;
    e.mbr = makeRectFromPoint(p);
    e.index = -1;
    return e;
}

// crea un nodo a partir de un rango de entradas (sin calcular MBR)
static Node makeNodeFromEntries(const vector<Child>& entries, int l, int r) {
    Node node{};
    node.k = r - l;

    for (int i = 0; i < node.k; i++) {
        node.hijos[i] = entries[l + i];
    }

    return node;
}

// convierte un vector de puntos en un vector de entradas (Child)
static vector<Child> pointsToEntries(const vector<Point>& points) {
    vector<Child> entries;
    entries.reserve(points.size());

    for (const Point& p : points) {
        entries.push_back(makeChildFromPoint(p));
    }

    return entries;
}

// construye un nivel del árbol a partir de las entradas actuales, devolviendo las entradas del nivel padre
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

// Construye el árbol Nearest-X a partir de los puntos dados, devolviendo el vector de nodos (con raíz en tree[0])
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

