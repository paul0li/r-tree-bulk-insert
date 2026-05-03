// Experimento 5.1: construcción de árboles para cada N ∈ {2^15..2^24}
// Uso: ./build.out <random.bin> <europa.bin> <output_dir>
// Salida CSV por stdout: dataset,method,N,time_ms
// Los árboles .bin quedan en output_dir con nombre: {dataset}_{method}_{N}.bin
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string>
#include "utils.hpp"
#include "nearest_x/nearest_x.hpp"
#include "str/str.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Uso: " << argv[0] << " <random.bin> <europa.bin> <output_dir>\n";
        return 1;
    }

    string randomPath = argv[1];
    string europaPath = argv[2];
    string outDir     = argv[3];

    // Valores de N pedidos en el enunciado
    vector<int> Ns;
    for (int exp = 15; exp <= 24; exp++)
        Ns.push_back(1 << exp);

    struct DatasetCfg { string name; string path; };
    vector<DatasetCfg> datasets = {
        {"random", randomPath},
        {"europa", europaPath}
    };

    cout << "dataset,method,N,time_ms\n";

    for (auto& ds : datasets) {
        // Leemos el máximo de puntos una sola vez para no releer el archivo en cada N
        int maxN = 1 << 24;
        vector<Point> allPoints = readPoints(ds.path, maxN);

        for (int N : Ns) {
            if (N > (int)allPoints.size()) {
                cerr << "Advertencia: " << ds.name << " tiene solo " << allPoints.size()
                     << " puntos, se omite N=" << N << "\n";
                continue;
            }

            vector<Point> points(allPoints.begin(), allPoints.begin() + N);

            // --- Nearest-X ---
            {
                auto start = chrono::high_resolution_clock::now();
                vector<Node> tree = buildNearestX(points);
                auto end   = chrono::high_resolution_clock::now();
                double ms  = chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0;

                string outPath = outDir + "/" + ds.name + "_nearestx_" + to_string(N) + ".bin";
                ofstream file(outPath, ios::binary);
                writeTree(file, tree);

                cout << ds.name << ",nearestx," << N << "," << ms << "\n";
                cout.flush();
                cerr << "[OK] " << outPath << " (" << tree.size() << " nodos)\n";
            }

            // --- STR ---
            {
                auto start = chrono::high_resolution_clock::now();
                vector<Node> tree = buildSTR(points);
                auto end   = chrono::high_resolution_clock::now();
                double ms  = chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0;

                string outPath = outDir + "/" + ds.name + "_str_" + to_string(N) + ".bin";
                ofstream file(outPath, ios::binary);
                writeTree(file, tree);

                cout << ds.name << ",str," << N << "," << ms << "\n";
                cout.flush();
                cerr << "[OK] " << outPath << " (" << tree.size() << " nodos)\n";
            }
        }
    }

    return 0;
}
