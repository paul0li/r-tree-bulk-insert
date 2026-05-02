#pragma once

struct Point {
    float x, y;
};

struct Rectangle {
    float x1, x2, y1, y2;
};

const int B = 204;

struct Child {
    Rectangle mbr;  // 16 bytes (4 floats)
    int index;      // 4 bytes (-1 si hoja)
};

struct Node {
    int k;           // 4 bytes
    Child hijos[B];  // 204 * 20 = 4080 bytes
    char pad[12];    // 12 bytes → total 4096
};
