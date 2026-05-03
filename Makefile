# Compilador y banderas de optimización
CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17 -I include/

# Nombre de los ejecutables que vamos a generar
TARGETS = build.out search.out

# Regla por defecto: compilar todo
all: $(TARGETS)

# 1. Regla para compilar el creador de árboles
build.out: src/main.cpp src/nearest_x/nearest_x.cpp src/str/str.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# 2. Regla para compilar el motor de búsqueda
search.out: src/search_value/search_value.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Regla para limpiar el proyecto antes de entregarlo (borra los .out)
clean:
	rm -f *.out