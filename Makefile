# Compilador y banderas de optimización
CXX = g++
CXXFLAGS = -O3 -Wall -std=c++11

# Nombre de los ejecutables que vamos a generar
TARGETS = main.out search_value.out

# Regla por defecto: compilar todo
all: $(TARGETS)

# 1. Regla para compilar el creador de árboles (junta el main, nearest_x y str)
main.out: src/main.cpp src/nearest_x.cpp src/str.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# 2. Regla para compilar tu motor de búsqueda e I/O
search_value.out: src/search_value.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Regla para limpiar el proyecto antes de entregarlo (borra los .out)
clean:
	rm -f *.out