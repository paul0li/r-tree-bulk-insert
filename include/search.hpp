#pragma once
#include "utils.hpp"
#include <vector>

using namespace std;

/// Búsqueda recursiva en disco: lee el nodo `nodeIndex`, y por cada hijo cuyo MBR intersecta `query`
/// baja al hijo (lectura de otro bloque) o, si es hoja (index -1), agrega el punto a `results`.
/// @param file       Archivo del árbol abierto en modo binario (layout: nodo i en offset i * sizeof(Node)).
/// @param nodeIndex  Índice del nodo a visitar (0 = raíz).
/// @param query      Rectángulo de consulta (x1 <= x2, y1 <= y2).
/// @param results    Acumulador in-out: puntos cuyo MBR intersecta `query` (en hojas equivale a estar dentro).
/// @param ioCount    Contador in-out: se incrementa en 1 por cada `readNode` (una lectura de bloque).
/// @return void      Sin valor; modifica `results` e `ioCount`.
inline void searchRTree(ifstream& file, int nodeIndex, const Rectangle& query, vector<Point>& results, int& ioCount) {
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
