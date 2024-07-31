all:
	g++ -I src/include -L src/lib -o checker Checker.cpp -o game Game.cpp  -o texture TextureLoader.cpp -o main main.cpp -lmingw32 -lSDL2main -lSDL2