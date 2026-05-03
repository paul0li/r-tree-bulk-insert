#pragma once

/// Punto en el plano (2D), coordenadas en precisión simple.
struct Point {
    float x; ///< Coordenada horizontal.
    float y; ///< Coordenada vertical.
};

/// Rectángulo mínimo alineado a ejes (MBR): límites en X e Y.
/// Convención x1 <= x2, y1 <= y2; en hoja es punto degenerado (x1==x2, y1==y2).
struct Rectangle {
    float x1; ///< Límite inferior en X.
    float x2; ///< Límite superior en X.
    float y1; ///< Límite inferior en Y.
    float y2; ///< Límite superior en Y.
};

/// Fan-out máximo del R-tree: cantidad de hijos por nodo (entre 1 y este valor).
/// Con valor 204 el struct Node ocupa 4096 bytes (un bloque).
const int B = 204;

/// Entrada en un nodo: MBR del hijo + índice en el arreglo serializado (simula bloque en disco).
struct Child {
    Rectangle mbr; ///< MBR del hijo (cuatro float).
    int index;     ///< Índice del nodo hijo en el vector, o -1 si es hoja (punto).
};

/// Nodo del R-tree; tamaño fijo de un bloque de disco (4096 bytes).
struct Node {
    int k;           ///< Cantidad de hijos activos (1 .. B).
    Child hijos[B];  ///< Entradas; solo hijos[0 .. k-1] están en uso.
    char pad[12];    ///< Relleno para que sizeof(Node) sea exactamente 4096.
};
