PROJECT = tut05
LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
CFLAGS = -std=c99 -Wall -Wno-missing-braces -Wno-unused-value -Wno-pointer-sign -D_DEFAULT_SOURCE $(PROJECT_CUSTOM_FLAGS)
CPPFLAGS = -Wall -Wno-missing-braces -Wno-unused-value -D_DEFAULT_SOURCE $(PROJECT_CUSTOM_FLAGS)
CC = clang
INCLUDE_PATHS += -I. -I../src
SRC_PATH = ../src
BUILD_PATH = build

.PHONY: clean

$(PROJECT): $(BUILD_PATH)/main.o $(BUILD_PATH)/glad.o $(BUILD_PATH)/shader.o $(BUILD_PATH)/texture.o
	$(CC) $(BUILD_PATH)/main.o \
	$(BUILD_PATH)/shader.o \
	$(BUILD_PATH)/texture.o \
	$(BUILD_PATH)/glad.o \
	$(LDFLAGS) -o $(BUILD_PATH)/$(PROJECT)

$(BUILD_PATH)/main.o: main.cpp
	$(CC) -c main.cpp $(CPPFLAGS) $(INCLUDE_PATHS) -o $(BUILD_PATH)/main.o

$(BUILD_PATH)/shader.o: $(SRC_PATH)/shader.cpp
	$(CC) -c $(SRC_PATH)/shader.cpp $(CPPFLAGS) $(INCLUDE_PATHS) -o $(BUILD_PATH)/shader.o

$(BUILD_PATH)/texture.o: $(SRC_PATH)/texture.cpp
	$(CC) -c $(SRC_PATH)/texture.cpp $(CPPFLAGS) $(INCLUDE_PATHS) -o $(BUILD_PATH)/texture.o

$(BUILD_PATH)/glad.o: $(SRC_PATH)/glad.c
	$(CC) -c $(SRC_PATH)/glad.c $(CFLAGS) $(INCLUDE_PATHS) -o $(BUILD_PATH)/glad.o

clean:
	rm $(BUILD_PATH)/*
	@echo Cleaning done
