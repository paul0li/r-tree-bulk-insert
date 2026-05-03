# Tarea 1 — Bulk-loading de R-trees

## Descripción

Implementación de un R-tree con memoria secundaria simulada mediante archivos binarios. La construcción usa dos técnicas de bulk-loading (Nearest-X y STR) para comparar eficiencia en consultas de rango minimizando lecturas a disco (I/O).

---

## Requisitos

- g++ con soporte C++17
- make

---

## Compilación

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

Si quieres construir y consultar un árbol desde tu propio código, incluye los headers y usa directamente:

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

## Estructura del Proyecto

```
.
├── include/
│   ├── structs.hpp       # Estructuras Point, Rectangle, Child, Node (4096 bytes)
│   ├── utils.hpp         # Funciones auxiliares: MBR, lectura/escritura binaria, intersección
│   └── search.hpp        # Búsqueda recursiva en disco (searchRTree)
├── src/
│   ├── build_experiment.cpp          # Experimento 5.1: construye árboles para todos los N
│   ├── main.cpp                      # Experimento 5.2: consultas por rectángulo
│   ├── nearest_x/
│   │   ├── nearest_x.hpp             # Cabecera de Nearest-X
│   │   └── nearest_x.cpp             # Implementación de buildNearestX
│   └── str/
│       ├── str.hpp                   # Cabecera de STR
│       └── str.cpp                   # Implementación de buildSTR
├── datos/
│   ├── random.bin        # Dataset aleatorio uniforme en [0,1]×[0,1]
│   └── europa.bin        # Dataset de edificios en Europa (normalizado a [0,1]×[0,1])
└── Makefile
```

---

## Estructuras principales (`include/structs.hpp`)

- `Point`: par de coordenadas (x, y).
- `Rectangle`: MBR con límites (x1, x2, y1, y2).
- `Child`: entrada de nodo — MBR del hijo + índice en el arreglo serializado (-1 si es hoja).
- `Node`: bloque de disco de exactamente 4096 bytes. Contiene `k` hijos activos (máximo B=204) y padding.

---

## Formato del archivo binario del árbol

Cada nodo ocupa exactamente 4096 bytes (un bloque). El nodo raíz siempre está en la posición 0. Para leer el nodo con índice `i`, se hace seek a `i * 4096` bytes desde el inicio del archivo.
