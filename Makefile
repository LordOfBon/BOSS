CC=g++
EMCC=emcc

CFLAGS=-O3 -std=c++17 -flto -Wformat=0
LDFLAGS=-O3 -flto -pthread

LDFLAGS_SFML=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

INCLUDES=-Isrc -Isrc/BOSS -Isrc/json -Isrc/search -Isrc/test -Isrc/sfml

SRC_BOSS=$(wildcard src/BOSS/*.cpp src/search/*.cpp) 
OBJ_BOSS=$(SRC_BOSS:.cpp=.o)

SRC_EXPERIMENTS=$(wildcard src/experiments/*.cpp) 
OBJ_EXPERIMENTS=$(SRC_EXPERIMENTS:.cpp=.o)

SRC_SFML=$(wildcard src/sfml/*.cpp) 
OBJ_SFML=$(SRC_SFML:.cpp=.o)

SRC_TEST=$(wildcard src/test/*.cpp src/test/catch2/*.cpp) 
OBJ_TEST=$(SRC_TEST:.cpp=.o)

SRC_EMSCRIPTEN=$(wildcard src/emscripten/*.cpp) 
OBJ_EMSCRIPTEN=$(SRC_EMSCRIPTEN:.cpp=.o)


all:bin/BOSS_Experiments bin/BOSS_SFML bin/BOSS_Test

bin/BOSS_Experiments:$(OBJ_BOSS) $(OBJ_EXPERIMENTS) Makefile
	$(CC) $(OBJ_BOSS) $(OBJ_EXPERIMENTS) -o $@  $(LDFLAGS)

bin/BOSS_SFML:$(OBJ_BOSS) $(OBJ_SFML) Makefile
	$(CC) $(OBJ_BOSS) $(OBJ_SFML) -o $@  $(LDFLAGS) $(LDFLAGS_SFML)

bin/BOSS_Test:$(OBJ_BOSS) $(OBJ_TEST) Makefile
	$(CC) $(OBJ_BOSS) $(OBJ_TEST) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 

clean:
	rm -f bin/BOSS_Experiments bin/BOSS_SFML bin/BOSS_Test src/BOSS/*.o src/experiments/*.o src/test/*.o src/sfml/*.o src/emscripten/*.o 

