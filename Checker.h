#pragma once
#include <vector>
#include <algorithm>
#include <SDL2/SDL.h>
#include "TextureLoader.h"


//klasa pionek
class Checker
{
public:
	//jaka dru�yna
	enum class Team {
		red,
		blue
	};


public:
	Checker(int setPosX, int setPosY, Team setTeam); //konstruktor
	static void loadTextures(SDL_Renderer* renderer); //funkcja �aduj�ca teksture pionka z pliku
	void draw(SDL_Renderer* renderer, int squareSizePixels); //funkcja rusuj�ca pionek na planszy
	void drawPossibleMoves(SDL_Renderer* renderer, int squareSizePixels, std::vector<Checker>& listCheckers, bool canOnlyMove2Squares);
	int checkHowFarCanMoveInAnyDirection(std::vector<Checker>& listCheckers);
	int tryToMoveToPosition(int x, int y, std::vector<Checker>& listCheckers, int& indexCheckerErase, bool canOnlyMove2Squares);
	int getPosX();
	int getPosY();
	Team getTeam();


private:
	void draw(SDL_Renderer* renderer, int squareSizePixels, int x, int y, bool drawTransparent = false);
	int checkHowFarCanMoveInDirection(int xDirection, int yDirection, std::vector<Checker>& listCheckers);
	Checker* findCheckerAtPosition(int x, int y, std::vector<Checker>& listCheckers);

	int posX, posY;
	Team team;
	bool isAKing = false;

	static SDL_Texture* textureRedKing, * textureRedRegular,
		* textureBlueKing, * textureBlueRegular;
};