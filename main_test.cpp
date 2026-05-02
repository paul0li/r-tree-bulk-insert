#include "include/utils.hpp"

// g++ -o main_test.out main_test.cpp
// ./main_test.out datos/random.bin 5
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Uso: " << argv[0] << " <archivo.bin> <N>\n";
        return 1;
    }

    string path = argv[1];
    int N = stoi(argv[2]);

    // Leer N puntos del binario
    vector<Point> points = readPoints(path, N);
    cout << "Puntos leídos: " << points.size() << "\n";

    // Convertir puntos a rectangulos de area 0
    vector<Rectangle> rects;
    for (const Point& p : points)
        rects.push_back(makeRectFromPoint(p));

    // Ordenar rectangulos por X
    sortByX(rects);

    cout << "Primeros 5 rectangulos ordenados por X:\n";
    for (int i = 0; i < 5 && i < (int)rects.size(); i++) {
        cout << "  r[" << i << "] = x1=" << rects[i].x1
             << " y1=" << rects[i].y1 << "\n";
    }

    // --- Test: crear nodo con los primeros B rectangulos y calcular su MBR ---
    cout << "\n--- Test nodo + MBR ---\n";

    Node node;
    node.k = min(B, (int)rects.size());
    for (int i = 0; i < node.k; i++) {
        node.hijos[i].mbr   = rects[i];
        node.hijos[i].index = -1;  // hojas
    }

    Rectangle mbr = calcMBR(node);
    cout << "Nodo con " << node.k << " hijos\n";
    cout << "MBR: x=[" << mbr.x1 << ", " << mbr.x2 << "]"
         << " y=[" << mbr.y1 << ", " << mbr.y2 << "]\n";

    return 0;
}
