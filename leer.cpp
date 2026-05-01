#include <bits/stdc++.h>
using namespace std;

struct Point {
    float x, y;
};

// Lee los primeros N puntos del archivo binario.
// Cada punto: 2 floats consecutivos (x, y), 8 bytes total.
vector<Point> readPoints(const string& path, int N) {
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

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Uso: " << argv[0] << " <archivo.bin> <N>\n";
        return 1;
    }

    string path = argv[1];
    int N = stoi(argv[2]);

    vector<Point> points = readPoints(path, N);
    cout << "Puntos leídos: " << points.size() << endl;

    for (int i = 0; i < 5 && i < (int)points.size(); i++) {
        cout << "  p[" << i << "] = (" << points[i].x << ", " << points[i].y << ")\n";
    }

    return 0;
}
