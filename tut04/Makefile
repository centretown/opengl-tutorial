PROJECT = tut04
LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
CFLAGS = -std=c99 -Wall -Wno-missing-braces -Wno-unused-value -Wno-pointer-sign -D_DEFAULT_SOURCE $(PROJECT_CUSTOM_FLAGS)
CPPFLAGS = -Wall -Wno-missing-braces -Wno-unused-value -D_DEFAULT_SOURCE $(PROJECT_CUSTOM_FLAGS)
CC = clang
INCLUDE_PATHS += -I.

.PHONY: clean

$(PROJECT): build/main.o build/glad.o build/shader.o build/texture.o
	$(CC) build/main.o \
	build/shader.o \
	build/texture.o \
	build/glad.o \
	$(LDFLAGS) -o build/$(PROJECT)

build/main.o: main.cpp
	$(CC) -c main.cpp $(CPPFLAGS) $(INCLUDE_PATHS) -o build/main.o

build/shader.o: shader.cpp
	$(CC) -c shader.cpp $(CPPFLAGS) $(INCLUDE_PATHS) -o build/shader.o

build/texture.o: texture.cpp
	$(CC) -c texture.cpp $(CPPFLAGS) $(INCLUDE_PATHS) -o build/texture.o

build/glad.o: ../glad/glad.c
	$(CC) -c ../glad/glad.c $(CFLAGS) $(INCLUDE_PATHS) -o build/glad.o

clean:
	rm build/*
	@echo Cleaning done
