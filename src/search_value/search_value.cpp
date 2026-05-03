// Experimento 5.2: consultas por rectángulo sobre los 4 árboles con N=2^24
// Uso: ./search.out <trees_dir> [N=16777216]
// Salida CSV por stdout: dataset,method,N,s,avg_io,avg_points,stddev_points
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include "../../include/search.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <trees_dir> [N=16777216]\n";
        return 1;
    }

    string treesDir = argv[1];
    int N = (argc >= 3) ? stoi(argv[2]) : (1 << 24);

    // Valores de s pedidos en el enunciado
    vector<float> sList = {0.0025f, 0.005f, 0.01f, 0.025f, 0.05f};

    // 4 árboles: dataset x método
    struct TreeCfg { string dataset; string method; };
    vector<TreeCfg> treeCfgs = {
        {"random", "nearestx"},
        {"random", "str"},
        {"europa", "nearestx"},
        {"europa", "str"},
    };

    int numQueries = 100;

    // usamos rd y mt19937 para tener mejores numeros aleatorios
    random_device rd;
    mt19937 gen(rd());

    cout << "dataset,method,N,s,avg_io,avg_points,stddev_points\n";

    for (float s : sList) {
        // Generar las 100 consultas una sola vez por valor de s,
        // para que todos los árboles reciban las mismas queries
        uniform_real_distribution<float> dis(0.0f, 1.0f - s);
        vector<Rectangle> queries(numQueries);
        for (int i = 0; i < numQueries; i++) {
            float x1 = dis(gen);
            float y1 = dis(gen);
            queries[i] = {x1, x1 + s, y1, y1 + s};
        }

        for (auto& cfg : treeCfgs) {
            string treePath = treesDir + "/" + cfg.dataset + "_" + cfg.method
                              + "_" + to_string(N) + ".bin";

            ifstream file(treePath, ios::binary);
            if (!file.is_open()) {
                cerr << "Error al abrir el archivo del árbol: " << treePath << "\n";
                continue;
            }

            cerr << "Ejecutando " << numQueries << " consultas de tamaño " << s
                 << "x" << s << " en " << cfg.dataset << "/" << cfg.method << "...\n";

            long long totalIOs = 0;
            long long totalPointsFound = 0;
            long long totalPointsSq = 0;

            for (int i = 0; i < numQueries; i++) {
                vector<Point> results;
                int ioCount = 0;

                searchRTree(file, 0, queries[i], results, ioCount);

                int pts = (int)results.size();
                totalIOs += ioCount;
                totalPointsFound += pts;
                totalPointsSq += (long long)pts * pts;
            }

            double avgIO     = totalIOs        / (double)numQueries;
            double avgPts    = totalPointsFound / (double)numQueries;
            double variance  = (totalPointsSq  / (double)numQueries) - (avgPts * avgPts);
            double stddevPts = sqrt(max(0.0, variance));

            cout << cfg.dataset << "," << cfg.method << "," << N << ","
                 << s << "," << avgIO << "," << avgPts << "," << stddevPts << "\n";
            cout.flush();

            file.close();
        }
    }

    return 0;
}
