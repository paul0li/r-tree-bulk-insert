// Bonus 5.3: construye R-tree con europa_bonus.bin (lon/lat no normalizados),
// consulta un cuadrado alrededor de una ubicación y vuelca puntos a CSV.
// N fijo en 2^24 (igual que resto de la tarea).
// Uso: ./bonus.out <europa_bonus.bin> <lon_c> <lat_c> <half_side> <out_tree.bin> <out_points.csv>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string>
#include "utils.hpp"
#include "search.hpp"
#include "../str/str.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 7) {
        cerr << "Uso: " << argv[0]
             << " <europa_bonus.bin> <lon_c> <lat_c> <half_side> <out_tree.bin> <out_points.csv>\n";
        return 1;
    }

    const int N      = 1 << 24;
    string dataPath  = argv[1];
    float  lonC      = stof(argv[2]);
    float  latC      = stof(argv[3]);
    float  h         = stof(argv[4]);
    string outTree   = argv[5];
    string outCsv    = argv[6];

    vector<Point> points = readPoints(dataPath, N);

    auto t0 = chrono::high_resolution_clock::now();
    vector<Node> tree = buildSTR(points);
    auto t1 = chrono::high_resolution_clock::now();
    double buildMs = chrono::duration_cast<chrono::microseconds>(t1 - t0).count() / 1000.0;

    {
        ofstream treeFile(outTree, ios::binary);
        if (!treeFile) {
            cerr << "[bonus] Error: no se pudo abrir " << outTree << " para escritura.\n";
            return 1;
        }
        writeTree(treeFile, tree);
    }

    Rectangle query{ lonC - h, lonC + h, latC - h, latC + h };


    ifstream treeIn(outTree, ios::binary);
    if (!treeIn) {
        cerr << "[bonus] Error: no se pudo abrir " << outTree << " para lectura.\n";
        return 1;
    }

    vector<Point> results;
    int ioCount = 0;
    auto s0 = chrono::high_resolution_clock::now();
    searchRTree(treeIn, 0, query, results, ioCount);
    auto s1 = chrono::high_resolution_clock::now();
    double searchMs = chrono::duration_cast<chrono::microseconds>(s1 - s0).count() / 1000.0;

    cerr << "[bonus] Búsqueda: " << results.size() << " puntos, "
         << ioCount << " I/Os, " << searchMs << " ms\n";

    ofstream csv(outCsv);
    if (!csv) {
        cerr << "[bonus] Error: no se pudo abrir " << outCsv << " para escritura.\n";
        return 1;
    }
    csv << "x,y\n";
    for (const Point& p : results) {
        csv << p.x << "," << p.y << "\n";
    }
    cerr << "[bonus] Puntos guardados en " << outCsv << "\n";

    return 0;
}
