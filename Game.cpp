#include "Game.h"
#include<iostream>



Game::Game(SDL_Window* window, SDL_Renderer* renderer, int boardSizePixels) : 
    squareSizePixels(boardSizePixels / 8), gameModeCurrent(GameMode::playing) {
    //Run the game.
    if (window != nullptr && renderer != nullptr) {
        //Load the textures for the checkers.
        Checker::loadTextures(renderer);

        textureCheckerBoard = TextureLoader::loadTexture("Checker Board.bmp", renderer);
        textureTeamRedWon = TextureLoader::loadTexture("Team Red Won Text.bmp", renderer);
        textureTeamBlueWon = TextureLoader::loadTexture("blueWon.bmp", renderer);

        resetBoard();


        //Start the game loop and run until it's time to stop.
        bool running = true;
        while (running) {
            processEvents(running);
            draw(renderer);
        }


        //Deallocate the textures.
        TextureLoader::deallocateTextures();
    }
}



void Game::processEvents(bool& running) {
    bool mouseDownThisFrame = false;

    //Process events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_MOUSEBUTTONDOWN:
            mouseDownThisFrame = (mouseDownStatus == 0);
            if (event.button.button == SDL_BUTTON_LEFT)
                mouseDownStatus = SDL_BUTTON_LEFT;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                mouseDownStatus = SDL_BUTTON_RIGHT;
            break;
        case SDL_MOUSEBUTTONUP:
            mouseDownStatus = 0;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
                // Quit the game.
                case SDL_SCANCODE_ESCAPE:
                    running = false;
                    break;

                case SDL_SCANCODE_R:
                    resetBoard();
                    break;
            }
        }
    }


    //Process input from the mouse cursor.
    if (mouseDownThisFrame) {
        int mouseX = 0, mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);
        //Convert from the window's coordinate system to the game's coordinate system.
        int squareX = mouseX / squareSizePixels - 1;
        int squareY = mouseY / squareSizePixels - 1;

        if (gameModeCurrent == GameMode::playing)
            checkCheckersWithMouseInput(squareX, squareY);
    }
}



void Game::checkCheckersWithMouseInput(int x, int y) {
    if (x >= -1 && x < 8 && y >= -1 && y < 8) {
        if ((indexCheckerInPlay > -1 &&indexCheckerInPlay < listCheckers.size())==false ) {
            // If no checker is selected, try to find and select one at the input position.
            for (int count = 0; count < listCheckers.size(); count++) {
                Checker* checkerSelected = &listCheckers[count];
                if (checkerSelected->getPosX() == x && checkerSelected->getPosY() == y &&
                    checkerSelected->getTeam() == teamSelectedForGameplay) {
                    indexCheckerInPlay = count;
                    break;  // Stop searching once a checker is selected.
                }
            }
        }
        else {
            // Otherwise, attempt to move the selected checker.
            int indexCheckerErase = -1;
            int distanceMoved = listCheckers[indexCheckerInPlay].tryToMoveToPosition(x, y, listCheckers, indexCheckerErase, checkerInPlayCanOnlyMove2Squares);

            // If a checker needs to be erased, then erase it and update indexCheckerSelectedForGameplay.
            if (indexCheckerErase > -1 && indexCheckerErase < listCheckers.size()) {
                listCheckers.erase(listCheckers.begin() + indexCheckerErase);
                if (indexCheckerInPlay > indexCheckerErase)
                    indexCheckerInPlay--;
            }

            // Check how far the selected checker wants to move and attempt to do so.
            switch (distanceMoved) {
            case 0:
                indexCheckerInPlay = -1;
                if (checkerInPlayCanOnlyMove2Squares) {
                    checkerInPlayCanOnlyMove2Squares = false;
                    teamSelectedForGameplay = (teamSelectedForGameplay == Checker::Team::red ? Checker::Team::blue : Checker::Team::red);
                }
                break;
            case 1:
                if (checkerInPlayCanOnlyMove2Squares == false) {
                    indexCheckerInPlay = -1;
                    teamSelectedForGameplay = (teamSelectedForGameplay == Checker::Team::red ? Checker::Team::blue : Checker::Team::red);
                }
                break;
            case 2:
                // If it moved two squares, check if it can move two squares again.
                if (indexCheckerInPlay > -1 && indexCheckerInPlay < listCheckers.size() &&
                    listCheckers[indexCheckerInPlay].checkHowFarCanMoveInAnyDirection(listCheckers) == 2) {
                    // It can move two squares again, so don't deselect it and make sure it only moves two squares.
                    checkerInPlayCanOnlyMove2Squares = true;
                }
                else {
                    indexCheckerInPlay = -1;
                    checkerInPlayCanOnlyMove2Squares = false;
                    teamSelectedForGameplay = (teamSelectedForGameplay == Checker::Team::red ? Checker::Team::blue : Checker::Team::red);
                }
                break;
            }

            checkWin();
        }
    }
}



void Game::draw(SDL_Renderer* renderer) {
    
    //Clear the screen. 
    //Draw
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 51, 51, 255);
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if ((x + y) % 2 == 0) {
                SDL_Rect rect = {
                    x * squareSizePixels,
                    y * squareSizePixels,
                    squareSizePixels,
                    squareSizePixels
                };

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    // Draw the checkers.
    for (auto& checkerSelected : listCheckers)
        checkerSelected.draw(renderer, squareSizePixels);

    //If a checker is selected then draw it's possible moves.
    if (indexCheckerInPlay > -1 && indexCheckerInPlay < listCheckers.size())
        listCheckers[indexCheckerInPlay].drawPossibleMoves(renderer, squareSizePixels, listCheckers, checkerInPlayCanOnlyMove2Squares);


    //If the game has ended then draw an image that has a black overlay with white text that indicates the winner.
    //Select the correct texture to be drawn.
    SDL_Texture* textureDrawSelected = nullptr;

    switch (gameModeCurrent) {
    case GameMode::teamRedWon:      textureDrawSelected = textureTeamRedWon;    break;
    case GameMode::teamBlueWon:     textureDrawSelected = textureTeamBlueWon;   break;
    }

    //Draw the texture overlay if needed.
    if (textureDrawSelected != nullptr)
        SDL_RenderCopy(renderer, textureDrawSelected, NULL, NULL);


    //Send the image to the window.
    SDL_RenderPresent(renderer);
}




void Game::resetBoard() {
    //Reset the game variables.
    gameModeCurrent = GameMode::playing;
    listCheckers.clear();
    teamSelectedForGameplay = Checker::Team::red;

    //Loop through the entire board and place checkers in the black squares on the first and last three rows.
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 7; x += 2) {
            if (y == 0)
            {
                listCheckers.push_back(Checker(x , y, Checker::Team::red));
            }
            else listCheckers.push_back(Checker(x+1 , y, Checker::Team::red));
        }
    }

    for (int y = 4; y < 7; y++) {
        for (int x = -1; x < 7; x += 2) {
            listCheckers.push_back(Checker(x + (y % 2), y, Checker::Team::blue));
        }
    }
}



void Game::checkWin() {
    // Check all the teams to see if they have any moves left and store the combined result.
    int result =
        teamStillHasAtLeastOneMoveLeft(Checker::Team::red) << 1 |
        teamStillHasAtLeastOneMoveLeft(Checker::Team::blue) << 0;

    // Debug prints to identify the issue
    std::cout << "Result: " << result << std::endl;
    std::cout << "Red has move left: " << teamStillHasAtLeastOneMoveLeft(Checker::Team::red) << std::endl;
    std::cout << "Blue has move left: " << teamStillHasAtLeastOneMoveLeft(Checker::Team::blue) << std::endl;

    // Check the result to see if only one of the teams can move and if so then set the game mode accordingly.
    switch (result) {
    case (1 << 1):
        gameModeCurrent = GameMode::teamRedWon;
        break;
    case (1 << 0):
        gameModeCurrent = GameMode::teamBlueWon;
        break;
    default:
        // Check if there are no moves left for the current team.
        if (!teamStillHasAtLeastOneMoveLeft(teamSelectedForGameplay)) {
            gameModeCurrent = (teamSelectedForGameplay == Checker::Team::red) ? GameMode::teamBlueWon : GameMode::teamRedWon;
            std::cout << "Current team has no moves left. Game Over!" << std::endl;
        }
        else
            gameModeCurrent = GameMode::playing;
        break;
    }
}

bool Game::teamStillHasAtLeastOneMoveLeft(Checker::Team team) {
    // Debug print to check the team and the number of checkers
    std::cout << "Checking moves for Team: " << (team == Checker::Team::red ? "Red" : "Blue") << std::endl;
    std::cout << "Number of checkers for the team: " << std::count_if(listCheckers.begin(), listCheckers.end(),
        [team](Checker& checker) { return checker.getTeam() == team; }) << std::endl;

    // Check the input team to see if it has at least one checker that can move
    for (auto& checkerSelected : listCheckers) {
        if (checkerSelected.getTeam() == team &&
            checkerSelected.checkHowFarCanMoveInAnyDirection(listCheckers) > 0) {
            std::cout << "Team has at least one move left." << std::endl;
            return true;
        }
    }

    std::cout << "Team has no move left." << std::endl;
    return false;
}
