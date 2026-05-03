// este es un programa inicial iwi aún falta para cambiar!
// no juzgar TT
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include "../include/utils.hpp" 

using namespace std;

// ---------------------------------------------------------
// ALGORITMO DE BÚSQUEDA RECURSIVA EN DISCO
// ---------------------------------------------------------
void searchRTree(ifstream& file, int nodeIndex, const Rectangle& query, vector<Point>& results, int& ioCount) {
    // 1. Leer el nodo desde el disco usando la función de Pau
    Node node = readNode(file, nodeIndex);
    
    // 2. Cada vez que leemos un nodo, es un I/O (Lectura a disco)
    ioCount++; 

    // 3. Revisar cada hijo de este nodo
    for (int i = 0; i < node.k; i++) {
        // Usamos el 'intersects' de utils.hpp para ver si vale la pena entrar
        if (intersects(query, node.hijos[i].mbr)) {
            
            if (node.hijos[i].index == -1) {
                // CASO BASE: Es una hoja (un punto)
                // Como 'intersects' funciona perfecto para puntos (rectángulos de área 0),
                // si entra en el 'if', el punto está dentro de la consulta.
                Point p = {node.hijos[i].mbr.x1, node.hijos[i].mbr.y1};
                results.push_back(p);
            } else {
                // CASO RECURSIVO: Es un nodo interno
                // Llamamos a la función con el índice de este hijo
                searchRTree(file, node.hijos[i].index, query, results, ioCount);
            }
        }
    }
}

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

    // Configuración para generar rectángulos aleatorios
    random_device rd;
    mt19937 gen(rd());
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
        searchRTree(file, 0, query, results, ioCount);
        
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