// este es un programa inicial iwi aún falta para cambiar!
// no juzgar TT
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include "../include/search.hpp"

using namespace std;

// ---------------------------------------------------------
// MOTOR DE EXPERIMENTOS (Sección 5.2)
// Uso: ./search_experiment <trees_dir> [N=16777216]
// ---------------------------------------------------------
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

    // Configuración para generar rectángulos aleatorios
    // usamos un generador de numeros aleatorios mejor que solo rand() para evitar patrones predecibles
    // vimos en internet que era mejor usar rd y mt19937 para tener mejores numeros aleatorios, lo malo es que es un poco mas lento
    cout << "Generando consultas aleatorias...\n";
    random_device rd;
    cout << "Usando generador de números aleatorios: mt19937\n"; // esto es como para saber un poco mejor cuanto se esta demorando
    mt19937 gen(rd());

    cout << "dataset,method,N,s,avg_io,avg_points,stddev_points\n";

    for (auto& cfg : treeCfgs) {
        string treePath = treesDir + "/" + cfg.dataset + "_" + cfg.method
                          + "_" + to_string(N) + ".bin";

        // Abrir archivo binario usando ifstream (como dicta utils.hpp)
        ifstream file(treePath, ios::binary);
        if (!file.is_open()) {
            cerr << "Error al abrir el archivo del árbol: " << treePath << "\n";
            continue;
        }
        int rootIndex = 0;                       // La raíz siempre está en el índice 0

        for (float s : sList) {
            // Queremos generar rectángulos de tamaño s x s dentro del espacio [0,1]x[0,1]
            uniform_real_distribution<float> dis(0.0, 1.0 - s);

            long long totalIOs = 0;
            long long totalPointsFound = 0;
            long long totalPointsSq = 0;
            long long totalTime_us = 0;

            cerr << "Ejecutando " << numQueries << " consultas de tamaño " << s << "x" << s << "...\n";

            for (int i = 0; i < numQueries; i++) {
                // Generar cuadrado de consulta aleatorio
                float q_x1 = dis(gen);
                float q_y1 = dis(gen);
                Rectangle query = {q_x1, q_x1 + s, q_y1, q_y1 + s};

                vector<Point> results;
                int ioCount = 0;

                // Medir el tiempo de la búsqueda
                auto start = chrono::high_resolution_clock::now();

                // Empezamos la búsqueda siempre desde la raíz (offset 0)
                searchRTree(file, rootIndex, query, results, ioCount);

                auto end = chrono::high_resolution_clock::now();

                int pts = (int)results.size();
                totalTime_us += chrono::duration_cast<chrono::microseconds>(end - start).count();
                totalIOs += ioCount;
                totalPointsFound += pts;
                totalPointsSq += (long long)pts * pts;
            }

            // Calcular promedios y desviación estándar
            double avgIO     = totalIOs         / (double)numQueries;
            double avgPts    = totalPointsFound  / (double)numQueries;
            double variance  = (totalPointsSq / (double)numQueries) - (avgPts * avgPts);
            double stddevPts = sqrt(max(0.0, variance));

            cout << cfg.dataset << "," << cfg.method << "," << N << ","
                 << s << "," << avgIO << "," << avgPts << "," << stddevPts << "\n";
            cout.flush();
        }

        file.close();
    }

    return 0;
}
