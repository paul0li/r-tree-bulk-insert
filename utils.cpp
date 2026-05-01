#include <bits/stdc++.h>
using namespace std;

struct Point {
    float x, y;
};

struct Rectangle {
    float x1, x2, y1, y2;
};

// Crea un rectangulo a partir de 2 puntos (esquinas opuestas).
Rectangle makeRectangle(Point a, Point b) {
    return Rectangle{
        min(a.x, b.x), 
        max(a.x, b.x), 
        min(a.y, b.y), 
        max(a.y, b.y)
    };
}

// Crea rectangulo a partir de un punto.
Rectangle makeRectFromPoint(const Point& p) {
    return Rectangle{
        p.x, p.x,
        p.y, p.y
    };
}

bool compareByX(const Rectangle& a, const Rectangle& b) {
    return (a.x1 + a.x2) < (b.x1 + b.x2);
}

// Ordena vector de rectangulos por coordenada X del centro (x1+x2)/2.
void sortByX(vector<Rectangle>& rects) {
    sort(rects.begin(), rects.end(), compareByX);
}

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
