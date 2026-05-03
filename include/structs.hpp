#pragma once

/** Punto en el plano (2D), coordenadas en precisión simple. */
struct Point {
    float x; 
    float y; 
};

/**
 * Rectángulo alineado a los ejes (MBR): límites inferior y superior en X e Y.
 */
struct Rectangle {
    float x1; 
    float x2; 
    float y1; 
    float y2; 
};

/** Cantidad de hijos por nodo (1 … B). Con B = 204 el nodo ocupa 4096 bytes. */
const int B = 204;

/**
 * Entrada dentro de un nodo: par clave-valor según el enunciado.
 * La clave es el MBR del hijo, el valor es el índice del hijo en el arreglo secuencial.
 */
struct Child {
    Rectangle mbr;  // 16 bytes (4 floats)
    int index;      // 4 bytes (-1 si hoja)
};

/**
 * Nodo del R-tree. Tamaño fijo de un bloque de disco (4096 bytes).
 * Los primeros k slots de hijos son válidos, el resto no se usa.
 */
struct Node {
    int k;          
    Child hijos[B]; 
    char pad[12];    
};
