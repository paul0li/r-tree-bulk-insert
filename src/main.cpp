// este es un programa inicial iwi aún falta para cambiar!
// no juzgar TT
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include "../include/search.hpp"

using namespace std;

// ---------------------------------------------------------
// MOTOR DE EXPERIMENTOS
// ---------------------------------------------------------
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Uso: " << argv[0] << " <archivo_arbol.bin> <lado_consulta_s>\n";
        return 1;
    }

    string treePath = argv[1];
    float s = stof(argv[2]); // Tamaño del lado de la consulta

    // Abrir archivo binario usando ifstream (como dicta utils.hpp)
    ifstream file(treePath, ios::binary);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo del árbol: " << treePath << "\n";
        return 1;
    }
    int rootIndex = 0;                       // La raíz siempre está en el índice 0

    // Configuración para generar rectángulos aleatorios
    // usamos un generador de numeros aleatorios mejor que solo rand() para evitar patrones predecibles
    // vimos en internet que era mejor usar rd y mt19937 para tener mejores numeros aleatorios, lo malo es que es un poco mas lento
    cout << "Generando consultas aleatorias...\n";
    random_device rd;
    cout << "Usando generador de números aleatorios: mt19937\n"; // esto es como para saber un poco mejor cuanto se esta demorando
    mt19937 gen(rd());

    // Queremos generar rectángulos de tamaño s x s dentro del espacio [0,1]x[0,1]
    uniform_real_distribution<float> dis(0.0, 1.0 - s);

    int numQueries = 100;
    long long totalIOs = 0;
    long long totalPointsFound = 0;
    long long totalTime_us = 0;

    cout << "Ejecutando " << numQueries << " consultas de tamaño " << s << "x" << s << "...\n";

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

        totalTime_us += chrono::duration_cast<chrono::microseconds>(end - start).count();
        totalIOs += ioCount;
        totalPointsFound += results.size();
    }

    file.close();

    // Calcular promedios
    cout << "--------------------------------------\n";
    cout << "Resultados Promedio (sobre " << numQueries << " consultas):\n";
    cout << "Tamaño de consulta (s): " << s << " (" << (s*s*100) << "% del área)\n";
    cout << "Tiempo de búsqueda:     " << (totalTime_us / (double)numQueries) << " microsegundos\n";
    cout << "Lecturas a disco (I/O): " << (totalIOs / (double)numQueries) << "\n";
    cout << "Puntos encontrados:     " << (totalPointsFound / (double)numQueries) << "\n";
    cout << "--------------------------------------\n";

    return 0;
}
