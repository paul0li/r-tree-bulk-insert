# Tarea 1 — Bulk-loading de R-trees

## Descripción del Proyecto

Este proyecto implementa un R-tree simulando el modelo de memoria secundaria mediante el uso de archivos binarios. La construcción del árbol se realiza utilizando dos técnicas de Bulk-loading (carga masiva) con el objetivo de comparar qué partición espacial es más eficiente para realizar consultas de rango minimizando las lecturas a disco (I/O).

---

## Requisitos

- g++ con soporte C++17
- make

---

## Archivos del Proyecto

| Archivo | Descripción |
|---|---|
| `include/structs.hpp` | Definición de las estructuras Point, Rectangle, Child y Node (4096 bytes). |
| `include/utils.hpp` | Funciones auxiliares (cálculo de MBR, lectura/escritura binaria a disco, intersección, etc.). |
| `include/search.hpp` | Búsqueda recursiva en disco (searchRTree). |
| `src/nearest_x/nearest_x.hpp` | Cabecera del método Nearest-X. |
| `src/str/str.hpp` | Cabecera del método STR. |
| `src/nearest_x/nearest_x.cpp` | Implementación del algoritmo de Bulk-loading Nearest-X. |
| `src/str/str.cpp` | Implementación del algoritmo de Bulk-loading Sort-Tile-Recursive (STR). |
| `src/main.cpp` | Experimento 5.1: construye árboles para cada N ∈ {2¹⁵,...,2²⁴} y los serializa a disco. |
| `src/search_value/search_value.cpp` | Experimento 5.2: motor de búsqueda en disco, promedia I/Os y puntos sobre 100 consultas aleatorias. |
| `Makefile` | Instrucciones de compilación automatizada y limpieza. |

---

## Estructuras Principales (`include/structs.hpp`)

- `Point`: Par de coordenadas espaciales (x, y).
- `Rectangle`: MBR definido por límites inferior y superior (x1, x2, y1, y2).
- `Child`: Par clave-valor de un nodo que contiene el MBR del hijo y su index (posición en el arreglo serializado). Usa -1 para identificar las hojas.
- `Node`: Estructura que simula un bloque de disco. Contiene la cantidad de hijos k, un arreglo de B = 204 hijos, y un padding para pesar exactamente 4096 bytes.

---

## Compilación

Para compilar todo el proyecto (usando la bandera de optimización `-O3`), sitúate en la raíz del proyecto y ejecuta:

```bash
make
```

Genera dos ejecutables: `build.out` (construcción) y `search.out` (consultas).

Para limpiar:
```bash
make clean
```

---

## Ejecución

### Experimento 5.1 — Construcción de árboles

Construye 4 árboles para cada N ∈ {2¹⁵, ..., 2²⁴} y guarda los archivos binarios en `<output_dir>`.

```bash
mkdir -p trees
./build.out <random.bin> <europa.bin> <output_dir>
```

Ejemplo:
```bash
./build.out datos/random.bin datos/europa.bin trees/
```

Salida por stdout (CSV):
```
dataset,method,N,time_ms
random,nearestx,32768,12.3
random,str,32768,11.8
...
```

Para guardar resultados:
```bash
./build.out datos/random.bin datos/europa.bin trees/ > build_results.csv 2>build_log.txt
```

### Experimento 5.2 — Consultas

Lee los 4 árboles con N=2²⁴ desde disco y ejecuta 100 consultas aleatorias para cada s ∈ {0.0025, 0.005, 0.01, 0.025, 0.05}.

```bash
./search.out <trees_dir>
```

Ejemplo:
```bash
./search.out trees/
```

Salida por stdout (CSV):
```
dataset,method,N,s,avg_io,avg_points,stddev_points
random,nearestx,16777216,0.0025,14.2,104.3,8.1
...
```

Para guardar resultados:
```bash
./search.out trees/ > search_results.csv 2>search_log.txt
```

---

## Uso libre de las funciones

Si quieres construir y consultar un árbol desde tu propio código:

### Construcción

```cpp
#include "include/utils.hpp"
#include "src/nearest_x/nearest_x.hpp"
#include "src/str/str.hpp"

// Leer puntos desde archivo binario
vector<Point> points = readPoints("datos/random.bin", 100000);

// Construir árbol (elige uno)
vector<Node> tree = buildNearestX(points);
// vector<Node> tree = buildSTR(points);

// Guardar en disco
ofstream file("mi_arbol.bin", ios::binary);
writeTree(file, tree);
file.close();
```

### Consulta

```cpp
#include "include/search.hpp"

ifstream file("mi_arbol.bin", ios::binary);

Rectangle query = {0.2f, 0.3f, 0.4f, 0.5f}; // {x1, x2, y1, y2}
vector<Point> results;
int ioCount = 0;

searchRTree(file, 0, query, results, ioCount); // 0 = índice de la raíz

// results contiene los puntos dentro del rectángulo
// ioCount contiene la cantidad de lecturas a disco
```

---

## Formato del archivo binario del árbol

Cada nodo ocupa exactamente 4096 bytes (un bloque). El nodo raíz siempre está en la posición 0. Para leer el nodo con índice `i`, se hace seek a `i * 4096` bytes desde el inicio del archivo.
