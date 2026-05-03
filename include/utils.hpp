#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include "structs.hpp"
using namespace std;

/**
 * Construye el rectángulo mínimo que contiene dos puntos cualesquiera (esquinas opuestas).
 * @param a,b Puntos en el plano (no necesitan estar ordenados).
 * @return Rectangle con x1 ≤ x2, y1 ≤ y2 que los envuelve.
 */
inline Rectangle makeRectangle(Point a, Point b) {
    return Rectangle{
        min(a.x, b.x),
        max(a.x, b.x),
        min(a.y, b.y),
        max(a.y, b.y)
    };
}

/**
 * Convierte un punto en rectángulo degenerado (área cero), como exige el bulk-load inicial.
 * @param p Punto fuente.
 * @return Rectangle con x1 = x2 = p.x e y1 = y2 = p.y.
 */
inline Rectangle makeRectFromPoint(const Point& p) {
    return Rectangle{ p.x, p.x, p.y, p.y };
}

/**
 * Calcula el MBR que envuelve todos los hijos activos de un nodo.
 * @param node Nodo con k >= 1.
 * @return Rectangle que es la unión de los MBR de hijos[0 ... k-1].
 */
inline Rectangle calcMBR(const Node& node) {
    Rectangle mbr = node.hijos[0].mbr;
    for (int i = 1; i < node.k; i++) {
        mbr.x1 = min(mbr.x1, node.hijos[i].mbr.x1);
        mbr.x2 = max(mbr.x2, node.hijos[i].mbr.x2);
        mbr.y1 = min(mbr.y1, node.hijos[i].mbr.y1);
        mbr.y2 = max(mbr.y2, node.hijos[i].mbr.y2);
    }
    return mbr;
}

/** Comparador interno: orden de puntos por coordenada X (para sortPointsByX). */
inline bool _comparePointsByX(const Point& a, const Point& b) { return a.x < b.x; }
/** Comparador interno: orden de puntos por coordenada Y (para sortPointsByY). */
inline bool _comparePointsByY(const Point& a, const Point& b) { return a.y < b.y; }
/** Comparador interno: orden por centro en X usando (x1+x2), equivalente a ordenar por x_medio (para sortByX). */
inline bool _compareByX(const Rectangle& a, const Rectangle& b) { return (a.x1 + a.x2) < (b.x1 + b.x2); }
/** Comparador interno: orden por centro en Y usando (y1+y2) (para sortByY). */
inline bool _compareByY(const Rectangle& a, const Rectangle& b) { return (a.y1 + a.y2) < (b.y1 + b.y2); }

/**
 * Comparador para entradas Child según la coordenada X del centro del MBR (usa x1+x2 del rectángulo).
 */
inline bool compareEntriesByX(const Child& a, const Child& b) {
    return (a.mbr.x1 + a.mbr.x2) < (b.mbr.x1 + b.mbr.x2);
}

/**
 * Comparador para entradas Child según la coordenada Y del centro del MBR (usa y1+y2 del rectángulo).
 */
inline bool compareEntriesByY(const Child& a, const Child& b) {
    return (a.mbr.y1 + a.mbr.y2) < (b.mbr.y1 + b.mbr.y2);
}

/** Ordena puntos en orden no decreciente por X. */
inline void sortPointsByX(vector<Point>& points) { sort(points.begin(), points.end(), _comparePointsByX); }
/** Ordena puntos en orden no decreciente por Y. */
inline void sortPointsByY(vector<Point>& points) { sort(points.begin(), points.end(), _comparePointsByY); }

/** Ordena entradas Child por centro del MBR en X (Nearest-X / pasos previos a STR). */
inline void sortEntriesByX(vector<Child>& entries) {
    sort(entries.begin(), entries.end(), compareEntriesByX);
}

/** Ordena entradas Child por centro del MBR en Y. */
inline void sortEntriesByY(vector<Child>& entries) {
    sort(entries.begin(), entries.end(), compareEntriesByY);
}

/** Ordena rectángulos por centro en X. */
inline void sortByX(vector<Rectangle>& rects) { sort(rects.begin(), rects.end(), _compareByX); }
/** Ordena rectángulos por centro en Y. */
inline void sortByY(vector<Rectangle>& rects) { sort(rects.begin(), rects.end(), _compareByY); }

/**
 * Escribe el árbol completo a un flujo binario: un bloque por nodo, en orden del vector.
 * @param file Flujo de salida abierto en modo binario.
 * @param nodes Secuencia de nodos; el índice i corresponde al offset i · sizeof(Node) en archivo.
 */
inline void writeTree(ofstream& file, const vector<Node>& nodes) {
    for (const Node& node : nodes)
        file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
}

/**
 * Prueba de intersección entre dos rectángulos alineados a ejes.
 * @param a,b Rectángulos en convención x1 ≤ x2, y1 ≤ y2.
 * @return true si existe intersección de área (incluye borde y tangencia).
 */
inline bool intersects(const Rectangle& a, const Rectangle& b) {
    return a.x1 <= b.x2 && a.x2 >= b.x1 &&
           a.y1 <= b.y2 && a.y2 >= b.y1;
}

/**
 * Lee un nodo desde un archivo binario de árbol serializado (una lectura de sizeof(Node) bytes).
 * @param file Flujo de entrada abierto en modo binario.
 * @param index Índice del nodo (0 = raíz); la posición en bytes es index · sizeof(Node).
 * @return Nodo leído (sin validar éxito de lectura; conviene comprobar file.good() si es crítico).
 */
inline Node readNode(ifstream& file, int index) {
    Node node;
    file.seekg((long long)index * sizeof(Node));
    file.read(reinterpret_cast<char*>(&node), sizeof(Node));
    return node;
}

/**
 * Lee hasta N puntos desde un archivo binario: por punto, dos float consecutivos (x, y).
 * @param path Ruta al archivo .bin.
 * @param N Cantidad máxima de puntos a leer.
 * @return Vector con los puntos leídos (puede tener menos de N si EOF o error de apertura).
 */
inline vector<Point> readPoints(const string& path, int N) {
    vector<Point> points;
    ifstream file(path, ios::binary);
    if (!file) {
        cout << "No se pudo abrir: " << path << "\n";
        return points;
    }
    Point p;
    char buf[4];
    while ((int)points.size() < N) {
        if (!file.read(buf, 4)) break;
        memcpy(&p.x, buf, 4);
        if (!file.read(buf, 4)) break;
        memcpy(&p.y, buf, 4);
        points.push_back(p);
    }
    return points;
}
