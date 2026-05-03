#include "include/utils.hpp"
#include <chrono>

using namespace std;

vector<Node> buildNearestX(const vector<Point>& points);
vector<Node> buildSTR(const vector<Point>& points);

void writeTreeToFile(const vector<Node>& tree, const string& outPath) {
    ofstream file(outPath, ios::binary);

    if (!file) {
        cerr << "No se pudo crear archivo: " << outPath << "\n";
        return;
    }

    writeTree(file, tree);
    file.close();
}

bool validateTree(const vector<Node>& tree) {
    if (tree.empty()) {
        cerr << "ERROR: árbol vacío\n";
        return false;
    }

    cout << "sizeof(Node): " << sizeof(Node) << "\n";

    if (sizeof(Node) != 4096) {
        cerr << "ERROR: Node no pesa 4096 bytes\n";
        return false;
    }

    for (int i = 0; i < (int)tree.size(); i++) {
        if (tree[i].k < 1 || tree[i].k > B) {
            cerr << "ERROR: nodo " << i << " tiene k inválido: "
                 << tree[i].k << "\n";
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cout << "Uso:\n";
        cout << "  " << argv[0] << " <nearestx|str> <archivo.bin> <N> <salida_tree.bin>\n";
        return 1;
    }

    string method = argv[1];
    string path = argv[2];
    int N = stoi(argv[3]);
    string outPath = argv[4];

    vector<Point> points = readPoints(path, N);
    cout << "Puntos leídos: " << points.size() << "\n";

    vector<Node> tree;

    auto start = chrono::high_resolution_clock::now();

    if (method == "nearestx") {
        tree = buildNearestX(points);
    } else if (method == "str") {
        tree = buildSTR(points);
    } else {
        cerr << "Método inválido. Usa nearestx o str.\n";
        return 1;
    }

    auto end = chrono::high_resolution_clock::now();
    double seconds = chrono::duration<double>(end - start).count();

    cout << "Método: " << method << "\n";
    cout << "Tiempo construcción: " << seconds << " segundos\n";
    cout << "Nodos creados: " << tree.size() << "\n";
    cout << "Hijos raíz: " << tree[0].k << "\n";

    if (!validateTree(tree)) {
        return 1;
    }

    writeTreeToFile(tree, outPath);

    cout << "Árbol escrito en: " << outPath << "\n";
    cout << "OK\n";

    return 0;
}