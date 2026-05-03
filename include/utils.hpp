#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include "structs.hpp"
using namespace std;

/// Rectángulo mínimo que contiene dos puntos (esquinas opuestas).
/// @param a Punto en el plano (no necesita orden respecto a b).
/// @param b Punto en el plano (no necesita orden respecto a a).
/// @return Rectangle con x1 <= x2, y1 <= y2 que envuelve a ambos puntos.
inline Rectangle makeRectangle(Point a, Point b) {
    return Rectangle{
        min(a.x, b.x),
        max(a.x, b.x),
        min(a.y, b.y),
        max(a.y, b.y)
    };
}

/// Convierte un punto en rectángulo degenerado (área cero), formato inicial del bulk-load.
/// @param p Punto fuente.
/// @return Rectangle con x1 = x2 = p.x e y1 = y2 = p.y.
inline Rectangle makeRectFromPoint(const Point& p) {
    return Rectangle{ p.x, p.x, p.y, p.y };
}

/// Unión de los MBR de todos los hijos activos del nodo.
/// @param node Nodo con k >= 1.
/// @return Rectangle que cubre hijos[0 .. k-1].
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

/// Comparador interno: orden por coordenada X (usa sortPointsByX).
/// @param a,b Puntos a comparar.
/// @return true si a debe ir antes que b por X.
inline bool _comparePointsByX(const Point& a, const Point& b) { return a.x < b.x; }
/// Comparador interno: orden por coordenada Y (usa sortPointsByY).
/// @param a,b Puntos a comparar.
/// @return true si a debe ir antes que b por Y.
inline bool _comparePointsByY(const Point& a, const Point& b) { return a.y < b.y; }
/// Comparador interno: orden por centro en X vía (x1+x2) (usa sortByX).
/// @param a,b Rectángulos a comparar.
/// @return true si el centro en X de a es menor que el de b.
inline bool _compareByX(const Rectangle& a, const Rectangle& b) { return (a.x1 + a.x2) < (b.x1 + b.x2); }
/// Comparador interno: orden por centro en Y vía (y1+y2) (usa sortByY).
/// @param a,b Rectángulos a comparar.
/// @return true si el centro en Y de a es menor que el de b.
inline bool _compareByY(const Rectangle& a, const Rectangle& b) { return (a.y1 + a.y2) < (b.y1 + b.y2); }

/// Compara entradas Child según la coordenada X del centro del MBR (usa x1+x2).
/// @param a,b Entradas a comparar.
/// @return true si el centro X del MBR de a es menor que el de b.
inline bool compareEntriesByX(const Child& a, const Child& b) {
    return (a.mbr.x1 + a.mbr.x2) < (b.mbr.x1 + b.mbr.x2);
}

/// Compara entradas Child según la coordenada Y del centro del MBR (usa y1+y2).
/// @param a,b Entradas a comparar.
/// @return true si el centro Y del MBR de a es menor que el de b.
inline bool compareEntriesByY(const Child& a, const Child& b) {
    return (a.mbr.y1 + a.mbr.y2) < (b.mbr.y1 + b.mbr.y2);
}

/// Ordena puntos por X ascendente (in-place).
/// @param points Vector de puntos; queda modificado.
/// @return void (sin valor; ordena in-place).
inline void sortPointsByX(vector<Point>& points) { sort(points.begin(), points.end(), _comparePointsByX); }
/// Ordena puntos por Y ascendente (in-place).
/// @param points Vector de puntos; queda modificado.
/// @return void (sin valor; ordena in-place).
inline void sortPointsByY(vector<Point>& points) { sort(points.begin(), points.end(), _comparePointsByY); }

/// Ordena entradas Child por centro del MBR en X (Nearest-X y pasos de STR).
/// @param entries Vector de entradas; queda modificado.
/// @return void (sin valor; ordena in-place).
inline void sortEntriesByX(vector<Child>& entries) {
    sort(entries.begin(), entries.end(), compareEntriesByX);
}

/// Ordena entradas Child por centro del MBR en Y (p. ej. STR dentro de cada franja).
/// @param entries Vector de entradas; queda modificado.
/// @return void (sin valor; ordena in-place).
inline void sortEntriesByY(vector<Child>& entries) {
    sort(entries.begin(), entries.end(), compareEntriesByY);
}

/// Ordena rectángulos por centro en X (in-place).
/// @param rects Vector de rectángulos; queda modificado.
/// @return void (sin valor; ordena in-place).
inline void sortByX(vector<Rectangle>& rects) { sort(rects.begin(), rects.end(), _compareByX); }
/// Ordena rectángulos por centro en Y (in-place).
/// @param rects Vector de rectángulos; queda modificado.
/// @return void (sin valor; ordena in-place).
inline void sortByY(vector<Rectangle>& rects) { sort(rects.begin(), rects.end(), _compareByY); }

/// Escribe la secuencia de nodos al archivo: bloque i en offset i * sizeof(Node).
/// @param file Flujo de salida abierto en modo binario.
/// @param nodes Vector de nodos en orden (índice = posición lógica en disco).
/// @return void (sin valor; escribe bytes en file).
inline void writeTree(ofstream& file, const vector<Node>& nodes) {
    for (const Node& node : nodes)
        file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
}

/// Intersección no vacía entre rectángulos alineados a ejes (incluye borde).
/// @param a Rectángulo con x1 <= x2, y1 <= y2.
/// @param b Rectángulo con x1 <= x2, y1 <= y2.
/// @return true si se solapan o se tocan.
inline bool intersects(const Rectangle& a, const Rectangle& b) {
    return a.x1 <= b.x2 && a.x2 >= b.x1 &&
           a.y1 <= b.y2 && a.y2 >= b.y1;
}

/// Lee un nodo del archivo serializado (una lectura de sizeof(Node) bytes).
/// @param file Flujo de entrada en modo binario.
/// @param index Índice del nodo (0 = raíz); seek a index * sizeof(Node).
/// @return Nodo leído. No valida éxito de lectura; usar file.good() si hace falta.
inline Node readNode(ifstream& file, int index) {
    Node node;
    file.seekg((long long)index * sizeof(Node));
    file.read(reinterpret_cast<char*>(&node), sizeof(Node));
    return node;
}

/// Lee hasta N puntos en formato binario: por punto, dos float (x, y) consecutivos.
/// @param path Ruta al archivo .bin.
/// @param N Máximo de puntos a leer.
/// @return Vector con los puntos leídos (menos de N si EOF o fallo de apertura).
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
