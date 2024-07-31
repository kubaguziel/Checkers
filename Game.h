#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include "Checker.h"
#include "TextureLoader.h"



class Game
{
private:
	enum class GameMode {
		playing,
		teamRedWon,
		teamBlueWon,
	} gameModeCurrent;


public:
	Game(SDL_Window* window, SDL_Renderer* renderer, int boardSizePixels);

	~Game() {
		// Deallocate textures if they are not nullptr.
		if (textureCheckerBoard != nullptr) {
			SDL_DestroyTexture(textureCheckerBoard);
			textureCheckerBoard = nullptr;
		}

		if (textureTeamRedWon != nullptr) {
			SDL_DestroyTexture(textureTeamRedWon);
			textureTeamRedWon = nullptr;
		}

		if (textureTeamBlueWon != nullptr) {
			SDL_DestroyTexture(textureTeamBlueWon);
			textureTeamBlueWon = nullptr;
		}
	}

private:

	void processEvents(bool& running);
	void checkCheckersWithMouseInput(int x, int y);
	void draw(SDL_Renderer* renderer);
	void resetBoard();
	void checkWin();
	bool teamStillHasAtLeastOneMoveLeft(Checker::Team team);

	std::vector<Checker> listCheckers;
	int indexCheckerInPlay = -1;
	bool checkerInPlayCanOnlyMove2Squares = false;
	Checker::Team teamSelectedForGameplay = Checker::Team::red;


	int mouseDownStatus = 0;

	SDL_Texture* textureCheckerBoard = nullptr;
	SDL_Texture* textureTeamRedWon = nullptr,
		* textureTeamBlueWon = nullptr;

	//The size of each squares on the board in pixels.
	const int squareSizePixels;
};