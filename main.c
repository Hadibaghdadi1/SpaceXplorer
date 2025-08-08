/**
 * SpaceXplorer Game
 * Main program entry point and game loop
 */

/* Standard input/output functions (printf, scanf, etc.) */
#include <stdio.h>
/* Memory allocation functions (malloc, free, etc.) */
#include <stdlib.h>
/* String manipulation functions (strcpy, strcmp, etc.) */
#include <string.h>
/* Time functions for random number generator seeding */
#include <time.h>
/* Game-specific declarations and structures */
#include "game.h"

/**
 * Main program entry point
 * Initializes the game, runs the main game loop, and displays end game message
 */
int main() {
    /* Seed random number generator with current time for varied gameplay */
    srand((unsigned int)time(NULL));
    
    /* Display welcome screen and introduction */
    displayWelcomeMessage();
    
    /* Create and initialize game state with player input */
    Game game;
    initGame(&game);
    
    /* Main game loop - continues until game over condition is reached */
    while (!game.isGameOver) {
        /* Render the current game state to the screen */
        renderWorld(&game);
        /* Process player input for the current turn */
        handleInput(&game);
        /* Update game state (placeholder for future features) */
        updateGame(&game);
    }
    
    /* Clean up memory - commented out because it's not being used in this version
       but would be necessary if dynamic memory allocation was used */
    // cleanupGame(&game);
    
    /* Display game over or victory screen and save score */
    displayEndGameMessage(&game);
    
    return 0;
} 