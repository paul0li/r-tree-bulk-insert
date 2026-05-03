#include "utils.hpp"
#include "str.hpp"
#include <cmath>

using namespace std;

static Child makeChildFromPoint(const Point& p) {
    Child e;
    e.mbr = makeRectFromPoint(p);
    e.index = -1;
    return e;
}

static Node makeNodeFromEntries(const vector<Child>& entries, int l, int r) {
    Node node{};
    node.k = r - l;

    for (int i = 0; i < node.k; i++) {
        node.hijos[i] = entries[l + i];
    }

    return node;
}

static vector<Child> pointsToEntries(const vector<Point>& points) {
    vector<Child> entries;
    entries.reserve(points.size());

    for (const Point& p : points) {
        entries.push_back(makeChildFromPoint(p));
    }

    return entries;
}

static vector<Child> buildSTRLevel(vector<Child>& current, vector<Node>& tree) {
    // Ordenar todo por X
    sort(current.begin(), current.end(), compareEntriesByX);

    int n = (int)current.size();

    // Cantidad aproximada de nodos que se crearán en este nivel
    int numNodes = (n + B - 1) / B;

    // S = ceil(sqrt(numNodes))
    int S = (int)ceil(sqrt((double)numNodes));

    // Cada franja vertical tendrá aprox S * B entradas
    int stripeSize = S * B;

    vector<Child> parentEntries;
    parentEntries.reserve(numNodes);

    // Recorrer franjas verticales
    for (int stripeStart = 0; stripeStart < n; stripeStart += stripeSize) {
        int stripeEnd = min(stripeStart + stripeSize, n);

        // Dentro de la franja, ordenar por Y
        sort(
            current.begin() + stripeStart,
            current.begin() + stripeEnd,
            compareEntriesByY
        );

        // Dentro de la franja, agrupar de a B
        for (int i = stripeStart; i < stripeEnd; i += B) {
            int l = i;
            int r = min(i + B, stripeEnd);

            Node node = makeNodeFromEntries(current, l, r);

            int nodeIndex = (int)tree.size();
            tree.push_back(node);

            Child parent;
            parent.mbr = calcMBR(node);
            parent.index = nodeIndex;

            parentEntries.push_back(parent);
        }
    }

    return parentEntries;
}

vector<Node> buildSTR(const vector<Point>& points) {
    vector<Node> tree;

    // Reservamos la posición 0 para la raíz
    tree.push_back(Node{});

    vector<Child> current = pointsToEntries(points);

    while ((int)current.size() > B) {
        current = buildSTRLevel(current, tree);
    }

    // Crear raíz en tree[0]
    tree[0] = makeNodeFromEntries(current, 0, (int)current.size());

    return tree;
}