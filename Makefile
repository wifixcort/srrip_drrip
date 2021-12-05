# Makefile
#
# Documentación en http://www.gnu.org/software/make/
#
# Un makefile contiene reglas, que indican como se procesan porciones 
# del programa y las dependencias de cada una
#
# Cada regla tiene la siguiente sintaxis:
#
# target: dependencias
# 	comandos

# También pueden definirse variables

# Ejecutable final
PROG=cache

# Archivos objeto necesarios para construir el ejecutable final
OBJS=cache.o

# Compilador
CC=g++

# Banderas de compilación
CFLAGS=-c -g -Wall

# Banderas de enlazado
LDFLAGS=-g -Wall

# Regla de creación del ejecutable final
$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

# Reglas de compilación, una por archivo .o

# Ejemplo
# archivo1.o: archivo1.c archivo1.h archivo2.h
#	$(CC) -o $@ $< $(CFLAGS)

cache.o: cache.cc
	$(CC) -o $@ $< $(CFLAGS)

# Los target PHONY son targets que realizan funciones miscelaneas, no 
# construyen archivos nuevos
.PHONY: clean

# Limpieza. Elimina todos los archivos creados
clean:
	rm -f $(PROG) *.o

