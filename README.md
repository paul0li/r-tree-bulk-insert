# Tarea 1 — Bulk-loading de R-trees

## Compilar y ejecutar

```bash
g++ -o main_test.out main_test.cpp
./main_test.out datos/random.bin <N>
```

`N` = cantidad de puntos a leer (ej. `500`, `32768`).

---

## Archivos

| Archivo | Descripción |
|---|---|
| `utils.h` | Structs y funciones auxiliares compartidas |
| `main_test.cpp` | Test: lee puntos, convierte a rectángulos, ordena y calcula MBR |

---

## utils.h — Structs

### `Point`
```cpp
struct Point { float x, y; };
```
Par de coordenadas (x, y).

### `Rectangle`
```cpp
struct Rectangle { float x1, x2, y1, y2; };
```
MBR definido por límites inferior y superior en cada coordenada.

### `Child`
```cpp
struct Child { Rectangle mbr; int index; };
```
Par clave-valor de un nodo: MBR del hijo e índice de su posición en el arreglo de nodos (-1 si es hoja).

### `Node`
```cpp
struct Node { int k; Child hijos[B]; char pad[12]; };
```
Nodo del R-tree. `k` = cantidad de hijos activos. Tamaño exacto: 4096 bytes (= 1 bloque de disco). `B = 204`.

---

## utils.h — Funciones

### `readPoints(path, N)`
Lee los primeros `N` puntos del archivo binario en `path`.
Cada punto son 2 `float`s consecutivos (x, y), 8 bytes total.
```cpp
vector<Point> points = readPoints("datos/random.bin", 1000);
```

### `makeRectFromPoint(p)`
Convierte un punto en un rectángulo de área 0 (x1=x2, y1=y2).
```cpp
Rectangle r = makeRectFromPoint(p);
```

### `makeRectangle(a, b)`
Crea el MBR mínimo que contiene dos puntos (esquinas opuestas).
```cpp
Rectangle r = makeRectangle(p1, p2);
```

### `calcMBR(node)`
Calcula el MBR que contiene todos los hijos del nodo.
```cpp
Rectangle mbr = calcMBR(node);
```

### `sortPointsByX(points)` / `sortPointsByY(points)`
Ordena vector de puntos por coordenada X o Y.
```cpp
sortPointsByX(points);
```

### `sortByX(rects)` / `sortByY(rects)`
Ordena vector de rectángulos por coordenada X o Y del centro.
```cpp
sortByX(rects);
```
