#include "utils.hpp"
#include "str.hpp"
#include "nearest_x.cpp"
#include <cmath>

using namespace std;

static vector<Child> buildSTRLevel(vector<Child>& current, vector<Node>& tree) {
    // 1. Ordenar todo por X
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

    // 2. Recorrer franjas verticales
    for (int stripeStart = 0; stripeStart < n; stripeStart += stripeSize) {
        int stripeEnd = min(stripeStart + stripeSize, n);

        // 3. Dentro de la franja, ordenar por Y
        sort(
            current.begin() + stripeStart,
            current.begin() + stripeEnd,
            compareEntriesByY
        );

        // 4. Dentro de la franja, agrupar de a B
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