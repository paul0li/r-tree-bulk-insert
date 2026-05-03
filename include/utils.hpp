#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include "structs.hpp"
using namespace std;

inline Rectangle makeRectangle(Point a, Point b) {
    return Rectangle{
        min(a.x, b.x),
        max(a.x, b.x),
        min(a.y, b.y),
        max(a.y, b.y)
    };
}

inline Rectangle makeRectFromPoint(const Point& p) {
    return Rectangle{ p.x, p.x, p.y, p.y };
}

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

inline bool _comparePointsByX(const Point& a, const Point& b) { return a.x < b.x; }
inline bool _comparePointsByY(const Point& a, const Point& b) { return a.y < b.y; }
inline bool _compareByX(const Rectangle& a, const Rectangle& b) { return (a.x1 + a.x2) < (b.x1 + b.x2); }
inline bool _compareByY(const Rectangle& a, const Rectangle& b) { return (a.y1 + a.y2) < (b.y1 + b.y2); }

//dani:agregué estas funciones que comparan las entries (Child)
inline bool compareEntriesByX(const Child& a, const Child& b) {
    return (a.mbr.x1 + a.mbr.x2) < (b.mbr.x1 + b.mbr.x2);
}

inline bool compareEntriesByY(const Child& a, const Child& b) {
    return (a.mbr.y1 + a.mbr.y2) < (b.mbr.y1 + b.mbr.y2);
}

inline void sortPointsByX(vector<Point>& points) { sort(points.begin(), points.end(), _comparePointsByX); }
inline void sortPointsByY(vector<Point>& points) { sort(points.begin(), points.end(), _comparePointsByY); }

//dani:agregué estas funciones para ordenar por coordenada X o Y un vector de rectangulos
inline void sortEntriesByX(vector<Child>& entries) {
    sort(entries.begin(), entries.end(), compareEntriesByX);
}

inline void sortEntriesByY(vector<Child>& entries) {
    sort(entries.begin(), entries.end(), compareEntriesByY);
}
inline void sortByX(vector<Rectangle>& rects) { sort(rects.begin(), rects.end(), _compareByX); }
inline void sortByY(vector<Rectangle>& rects) { sort(rects.begin(), rects.end(), _compareByY); }

inline void writeTree(ofstream& file, const vector<Node>& nodes) {
    for (const Node& node : nodes)
        file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
}

// Dos rectangulos intersectan si NO se separan en X ni en Y.
inline bool intersects(const Rectangle& a, const Rectangle& b) {
    return a.x1 <= b.x2 && a.x2 >= b.x1 &&
           a.y1 <= b.y2 && a.y2 >= b.y1;
}

// Lee el nodo en posicion index del archivo binario.
inline Node readNode(ifstream& file, int index) {
    Node node;
    file.seekg((long long)index * sizeof(Node));
    file.read(reinterpret_cast<char*>(&node), sizeof(Node));
    return node;
}

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
