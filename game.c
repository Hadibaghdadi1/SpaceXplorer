/* Standard input/output functions (printf, scanf, etc.) */
#include <stdio.h>
/* Memory allocation functions (malloc, free, etc.) */
#include <stdlib.h>
/* String manipulation functions (strcpy, strcat, etc.) */
#include <string.h>
/* Time functions for random number generation */
#include <time.h>
/* Character handling functions (toupper, isalpha, etc.) */
#include <ctype.h>
/* Game-specific declarations and structures */
#include "game.h"

/* Fuel levels for each difficulty (Easy, Medium, Hard) */
const int FUEL_LEVELS[] = {500, 350, 200};
/* Fuel consumption rates for each move based on difficulty */
const int FUEL_CONSUMPTION[] = {1, 2, 3};
/* Number of collectible junk items for each difficulty level */
const int JUNK_COUNTS[] = {40, 30, 20};
/* Asteroid movement speeds for each difficulty level */
const int ASTEROID_SPEEDS[] = {1, 2, 3};
/* Score required to win at each difficulty level */
const int WIN_SCORES[] = {500, 750, 1000};

/* File path for game configuration settings */
const char* CONFIG_FILE = "config.txt";
/* File path for storing player high scores */
const char* LEADERBOARD_FILE = "leaderboard.txt";
/* File path for game introduction text */
const char* INTRO_FILE = "intro.txt";

/* Initialize the game with player info, difficulty settings, and game objects */
void initGame(Game* game) {
    /* Variable to store user's difficulty choice */
    char difficultyChar;
    
    /* Prompt user for player name and store it */
    printf("Enter your name (max %d characters): ", MAX_NAME_LENGTH - 1);
    fgets(game->playerName, MAX_NAME_LENGTH, stdin);
    /* Remove newline character from input */
    game->playerName[strcspn(game->playerName, "\n")] = 0;
    
    /* Loop until valid difficulty is selected */
    do {
        printf("Choose difficulty (E)asy, (M)edium, (H)ard: ");
        scanf(" %c", &difficultyChar);
        /* Convert to uppercase for case-insensitive comparison */
        difficultyChar = toupper(difficultyChar);
    } while (difficultyChar != 'E' && difficultyChar != 'M' && difficultyChar != 'H');
    
    /* Set difficulty based on user choice */
    switch (difficultyChar) {
        case 'E': game->difficulty = EASY; break;
        case 'M': game->difficulty = MEDIUM; break;
        case 'H': game->difficulty = HARD; break;
        default: game->difficulty = MEDIUM; break;
    }
    
    /* Clear input buffer */
    while (getchar() != '\n');
    
    /* Load world dimensions from config file */
    loadConfig(game);
    
    /* Allocate memory for game world */
    createWorld(game);
    
    /* Initialize player's spaceship position and stats */
    game->ship.position.x = game->worldWidth / 2;
    game->ship.position.y = game->worldHeight / 2;
    game->ship.fuel = FUEL_LEVELS[game->difficulty];
    game->ship.maxFuel = FUEL_LEVELS[game->difficulty];
    game->ship.health = 100;
    game->ship.maxHealth = 100;
    game->ship.metal = 0;
    game->ship.plastic = 0;
    game->ship.electronics = 0;
    game->ship.fuelCells = 0;
    
    /* Randomly choose which edge the asteroid will start from (0=top, 1=right, 2=bottom, 3=left) */
    int edge = rand() % 4;
    
    /* Set asteroid position and direction based on chosen edge */
    switch (edge) {
        case 0:
            /* Top edge */
            game->asteroid.position.x = rand() % game->worldWidth;
            game->asteroid.position.y = 0;
            game->asteroid.direction.x = (rand() % 3) - 1; /* -1, 0, or 1 */
            game->asteroid.direction.y = 1; /* Moving down */
            break;
        case 1:
            /* Right edge */
            game->asteroid.position.x = game->worldWidth - 1;
            game->asteroid.position.y = rand() % game->worldHeight;
            game->asteroid.direction.x = -1; /* Moving left */
            game->asteroid.direction.y = (rand() % 3) - 1; /* -1, 0, or 1 */
            break;
        case 2:
            /* Bottom edge */
            game->asteroid.position.x = rand() % game->worldWidth;
            game->asteroid.position.y = game->worldHeight - 1;
            game->asteroid.direction.x = (rand() % 3) - 1; /* -1, 0, or 1 */
            game->asteroid.direction.y = -1; /* Moving up */
            break;
        case 3:
            /* Left edge */
            game->asteroid.position.x = 0;
            game->asteroid.position.y = rand() % game->worldHeight;
            game->asteroid.direction.x = 1; /* Moving right */
            game->asteroid.direction.y = (rand() % 3) - 1; /* -1, 0, or 1 */
            break;
    }
    
    /* Ensure asteroid is moving (not stationary) */
    if (game->asteroid.direction.x == 0 && game->asteroid.direction.y == 0) {
        game->asteroid.direction.x = 1;
    }
    
    /* Set asteroid symbol for display */
    game->asteroid.symbol = 'A';
    
    /* Place impassable cells (obstacles) randomly in the world */
    for (int i = 0; i < IMPASSABLE_CELLS; i++) {
        int valid = 0;
        while (!valid) {
            int x = rand() % game->worldWidth;
            int y = rand() % game->worldHeight;
            
            /* Ensure obstacle doesn't overlap with ship or asteroid */
            if ((x != game->ship.position.x || y != game->ship.position.y) &&
                (x != game->asteroid.position.x || y != game->asteroid.position.y)) {
                
                int overlap = 0;
                /* Check if position overlaps with previously placed obstacles */
                for (int j = 0; j < i; j++) {
                    if (game->impassableCells[j].position.x == x && 
                        game->impassableCells[j].position.y == y) {
                        overlap = 1;
                        break;
                    }
                }
                
                /* If no overlap, place the obstacle */
                if (!overlap) {
                    game->impassableCells[i].position.x = x;
                    game->impassableCells[i].position.y = y;
                    game->impassableCells[i].symbol = '#';
                    valid = 1;
                }
            }
        }
    }
    
    /* Set junk count based on difficulty level */
    game->junkCount = JUNK_COUNTS[game->difficulty];
    
    /* Place junk items randomly in the world */
    for (int i = 0; i < game->junkCount; i++) {
        int valid = 0;
        while (!valid) {
            int x = rand() % game->worldWidth;
            int y = rand() % game->worldHeight;
            
            /* Ensure junk doesn't overlap with ship or asteroid */
            if ((x != game->ship.position.x || y != game->ship.position.y) &&
                (x != game->asteroid.position.x || y != game->asteroid.position.y)) {
                
                int overlap = 0;
                
                /* Check if position overlaps with obstacles */
                for (int j = 0; j < IMPASSABLE_CELLS; j++) {
                    if (game->impassableCells[j].position.x == x && 
                        game->impassableCells[j].position.y == y) {
                        overlap = 1;
                        break;
                    }
                }
                
                /* Check if position overlaps with other junk items */
                if (!overlap) {
                    for (int j = 0; j < i; j++) {
                        if (game->junkItems[j].position.x == x && 
                            game->junkItems[j].position.y == y) {
                            overlap = 1;
                            break;
                        }
                    }
                }
                
                /* If no overlap, place the junk item */
                if (!overlap) {
                    game->junkItems[i].position.x = x;
                    game->junkItems[i].position.y = y;
                    /* Randomly determine junk type (0-3) */
                    game->junkItems[i].type = rand() % 4;
                    game->junkItems[i].collected = 0;
                    
                    /* Set junk properties based on type */
                    switch (game->junkItems[i].type) {
                        case METAL:
                            game->junkItems[i].value = 10;
                            game->junkItems[i].symbol = 'M';
                            break;
                        case PLASTIC:
                            game->junkItems[i].value = 5;
                            game->junkItems[i].symbol = 'P';
                            break;
                        case ELECTRONICS:
                            game->junkItems[i].value = 15;
                            game->junkItems[i].symbol = 'E';
                            break;
                        case FUEL_CELL:
                            game->junkItems[i].value = 20;
                            game->junkItems[i].symbol = 'F';
                            break;
                    }
                    
                    valid = 1;
                }
            }
        }
    }
    
    /* Initialize game score and state */
    game->score = 0;
    game->isGameOver = 0;
    game->hasWon = 0;
}

/* Load game configuration from file or create with default values if file doesn't exist */
void loadConfig(Game* game) {
    /* Try to open config file for reading */
    FILE* file = fopen(CONFIG_FILE, "r");
    
    /* Set default minimum values for world dimensions */
    game->worldWidth = WORLD_MIN_SIZE;
    game->worldHeight = WORLD_MIN_SIZE;
    
    if (file != NULL) {
        /* Read world width and height from config file */
        fscanf(file, "width=%d\n", &game->worldWidth);
        fscanf(file, "height=%d\n", &game->worldHeight);
        
        /* Close the file after reading */
        fclose(file);
        
        /* Ensure values are not below the minimum allowed size */
        if (game->worldWidth < WORLD_MIN_SIZE) game->worldWidth = WORLD_MIN_SIZE;
        if (game->worldHeight < WORLD_MIN_SIZE) game->worldHeight = WORLD_MIN_SIZE;
    } else {
        /* If config file doesn't exist, create it with default values */
        file = fopen(CONFIG_FILE, "w");
        if (file != NULL) {
            fprintf(file, "width=%d\n", game->worldWidth);
            fprintf(file, "height=%d\n", game->worldHeight);
            fclose(file);
        }
    }
}

/* Allocate memory for the 2D game world */
void createWorld(Game* game) {
    /* Allocate memory for the rows (y-dimension) */
    game->world = (char**)malloc(game->worldHeight * sizeof(char*));
    /* Allocate memory for each column (x-dimension) in each row */
    for (int y = 0; y < game->worldHeight; y++) {
        game->world[y] = (char*)malloc(game->worldWidth * sizeof(char));
    }
}

/* Free all dynamically allocated memory used by the game */
void cleanupGame(Game* game) {
    /* Free each row of the world grid */
    for (int y = 0; y < game->worldHeight; y++) {
        free(game->world[y]);
    }
    /* Free the array of row pointers */
    free(game->world);
}

/* Draw the game world and display status information */
void renderWorld(Game* game) {
    /* Clear the console screen */
    system("cls");
    
    /* Initialize all world cells to empty space ('.') */
    for (int y = 0; y < game->worldHeight; y++) {
        for (int x = 0; x < game->worldWidth; x++) {
            game->world[y][x] = '.';
        }
    }
    
    /* Place the ship on the world */
    game->world[game->ship.position.y][game->ship.position.x] = 'S';
    
    /* Place the asteroid on the world */
    game->world[game->asteroid.position.y][game->asteroid.position.x] = game->asteroid.symbol;
    
    /* Place impassable cells (obstacles) on the world */
    for (int i = 0; i < IMPASSABLE_CELLS; i++) {
        int x = game->impassableCells[i].position.x;
        int y = game->impassableCells[i].position.y;
        game->world[y][x] = game->impassableCells[i].symbol;
    }
    
    /* Place uncollected junk items on the world */
    for (int i = 0; i < game->junkCount; i++) {
        if (!game->junkItems[i].collected) {
            int x = game->junkItems[i].position.x;
            int y = game->junkItems[i].position.y;
            game->world[y][x] = game->junkItems[i].symbol;
        }
    }
    
    /* Print the x-axis coordinates at the top */
    printf("\n   ");
    for (int x = 0; x < game->worldWidth; x++) {
        printf("%d", x % 10);
    }
    printf("\n");
    
    /* Print the world with y-axis coordinates */
    for (int y = 0; y < game->worldHeight; y++) {
        printf("%2d ", y % 100);
        for (int x = 0; x < game->worldWidth; x++) {
            printf("%c", game->world[y][x]);
        }
        printf("\n");
    }
    
    /* Display game status information */
    printf("\nFuel: %d/%d | Health: %d/%d | Score: %d\n", 
           game->ship.fuel, game->ship.maxFuel, 
           game->ship.health, game->ship.maxHealth, 
           game->score);
           
    /* Display available game controls */
    printf("\nControls: (W)Up (S)Down (A)Left (D)Right (Q)Quit (I)Inventory (U)Use items\n");
}

/* Process user input and execute corresponding game actions */
void handleInput(Game* game) {
    /* Variable to store the user's input command */
    char input;
    printf("\nEnter command: ");
    scanf(" %c", &input);
    /* Convert input to uppercase for case-insensitive comparison */
    input = toupper(input);
    
    /* Execute action based on user's input */
    switch (input) {
        case 'W':
            /* Move ship up */
            moveSpaceship(game, 0, -1);
            break;
        case 'S':
            /* Move ship down */
            moveSpaceship(game, 0, 1);
            break;
        case 'A':
            /* Move ship left */
            moveSpaceship(game, -1, 0);
            break;
        case 'D':
            /* Move ship right */
            moveSpaceship(game, 1, 0);
            break;
        case 'I':
            /* Show inventory/ship status */
            displayShipStatus(game);
            break;
        case 'U':
            /* Display options for using inventory items */
            printf("Choose item to use:\n");
            printf("1. Metal (Repair ship)\n");
            printf("2. Fuel Cell (Refuel ship)\n");
            printf("3. Cancel\n");
            
            /* Get user's item choice */
            int choice;
            scanf("%d", &choice);
            if (choice >= 1 && choice <= 2) {
                useJunk(game, choice);
            }
            break;
        case 'Q':
            /* Quit the game */
            game->isGameOver = 1;
            break;
    }
}

/* Move the player's spaceship in the specified direction */
void moveSpaceship(Game* game, int dx, int dy) {
    /* Calculate new position after movement */
    int newX = game->ship.position.x + dx;
    int newY = game->ship.position.y + dy;
    
    /* Check if new position is within world boundaries */
    if (newX >= 0 && newX < game->worldWidth && newY >= 0 && newY < game->worldHeight) {
        /* Flag to determine if movement is allowed */
        int canMove = 1;
        
        /* Check if new position overlaps with any impassable cell */
        for (int i = 0; i < IMPASSABLE_CELLS; i++) {
            if (newX == game->impassableCells[i].position.x && 
                newY == game->impassableCells[i].position.y) {
                canMove = 0;
                break;
            }
        }
        
        /* If movement is allowed, update ship position and process turn consequences */
        if (canMove) {
            /* Update ship position */
            game->ship.position.x = newX;
            game->ship.position.y = newY;
            
            /* Consume fuel based on difficulty level */
            game->ship.fuel -= FUEL_CONSUMPTION[game->difficulty];
            
            /* Check if out of fuel - game over condition */
            if (game->ship.fuel <= 0) {
                game->isGameOver = 1;
                game->hasWon = 0;
                return;
            }
            
            /* Move asteroid and check for collisions after player's move */
            moveAsteroid(game);
            
            /* Check if player collected any junk or reached win condition */
            checkCollisions(game);
        }
    }
}

/* Move the asteroid based on its current direction and check for collisions */
void moveAsteroid(Game* game) {
    /* Asteroid moves faster at higher difficulties */
    int speed = ASTEROID_SPEEDS[game->difficulty];
    
    /* Move the asteroid multiple times based on its speed */
    for (int i = 0; i < speed; i++) {
        /* Calculate new asteroid position */
        int newX = game->asteroid.position.x + game->asteroid.direction.x;
        int newY = game->asteroid.position.y + game->asteroid.direction.y;
        
        /* If asteroid hits horizontal world boundary, reverse horizontal direction */
        if (newX < 0 || newX >= game->worldWidth) {
            game->asteroid.direction.x *= -1;
            newX = game->asteroid.position.x + game->asteroid.direction.x;
        }
        
        /* If asteroid hits vertical world boundary, reverse vertical direction */
        if (newY < 0 || newY >= game->worldHeight) {
            game->asteroid.direction.y *= -1;
            newY = game->asteroid.position.y + game->asteroid.direction.y;
        }
        
        /* Check if asteroid would hit an impassable cell */
        int blocked = 0;
        for (int j = 0; j < IMPASSABLE_CELLS; j++) {
            if (newX == game->impassableCells[j].position.x && 
                newY == game->impassableCells[j].position.y) {
                blocked = 1;
                break;
            }
        }
        
        /* If asteroid would hit an obstacle, reverse its direction */
        if (blocked) {
            game->asteroid.direction.x *= -1;
            game->asteroid.direction.y *= -1;
            
            /* Ensure asteroid is not stationary after collision */
            if (game->asteroid.direction.x == 0 && game->asteroid.direction.y == 0) {
                game->asteroid.direction.x = 1;
            }
            
            /* Recalculate new position with reversed direction */
            newX = game->asteroid.position.x + game->asteroid.direction.x;
            newY = game->asteroid.position.y + game->asteroid.direction.y;
        }
        
        /* Update asteroid position */
        game->asteroid.position.x = newX;
        game->asteroid.position.y = newY;
        
        /* Check if asteroid hit the player - game over condition */
        if (game->asteroid.position.x == game->ship.position.x && 
            game->asteroid.position.y == game->ship.position.y) {
            game->isGameOver = 1;
            game->hasWon = 0;
            break;
        }
    }
}

/* Check for item collection and win condition after player moves */
void checkCollisions(Game* game) {
    /* Check if player has moved onto any uncollected junk items */
    for (int i = 0; i < game->junkCount; i++) {
        if (!game->junkItems[i].collected &&
            game->ship.position.x == game->junkItems[i].position.x && 
            game->ship.position.y == game->junkItems[i].position.y) {
            /* Process junk collection */
            collectJunk(game, i);
        }
    }
    
    /* Check if player has reached the winning score */
    if (game->score >= WIN_SCORES[game->difficulty]) {
        game->isGameOver = 1;
        game->hasWon = 1;
    }
}

/* Process the collection of a junk item by the player */
void collectJunk(Game* game, int index) {
    /* Mark the junk item as collected so it disappears from the world */
    game->junkItems[index].collected = 1;
    
    /* Add the junk's value to the player's score */
    game->score += game->junkItems[index].value;
    
    /* Process specific junk type collection */
    switch (game->junkItems[index].type) {
        case METAL:
            /* Increment metal count in inventory */
            game->ship.metal++;
            printf("Collected metal!\n");
            break;
        case PLASTIC:
            /* Increment plastic count in inventory */
            game->ship.plastic++;
            printf("Collected plastic!\n");
            break;
        case ELECTRONICS:
            /* Increment electronics count in inventory */
            game->ship.electronics++;
            printf("Collected electronics!\n");
            break;
        case FUEL_CELL:
            /* Increment fuel cells count in inventory */
            game->ship.fuelCells++;
            printf("Collected fuel cell!\n");
            break;
    }
    
    /* Wait for player to acknowledge collection before continuing */
    printf("Press Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

/* Use items from inventory to repair ship or refuel */
void useJunk(Game* game, int option) {
    switch (option) {
        case 1:
            /* Use metal to repair ship */
            if (game->ship.metal > 0) {
                /* Increase health by 10 points */
                game->ship.health += 10;
                /* Ensure health doesn't exceed maximum */
                if (game->ship.health > game->ship.maxHealth) {
                    game->ship.health = game->ship.maxHealth;
                }
                /* Consume one metal item from inventory */
                game->ship.metal--;
                printf("Ship repaired! Health: %d/%d\n", game->ship.health, game->ship.maxHealth);
            } else {
                printf("Not enough metal!\n");
            }
            break;
        case 2:
            /* Use fuel cell to refuel ship */
            if (game->ship.fuelCells > 0) {
                /* Increase fuel by 50 units */
                game->ship.fuel += 50;
                /* Ensure fuel doesn't exceed maximum */
                if (game->ship.fuel > game->ship.maxFuel) {
                    game->ship.fuel = game->ship.maxFuel;
                }
                /* Consume one fuel cell from inventory */
                game->ship.fuelCells--;
                printf("Ship refueled! Fuel: %d/%d\n", game->ship.fuel, game->ship.maxFuel);
            } else {
                printf("Not enough fuel cells!\n");
            }
            break;
    }
    /* Wait for player to acknowledge before continuing */
    printf("Press Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

/* Display detailed ship status and inventory information */
void displayShipStatus(Game* game) {
    /* Show ship status (fuel, health, score) */
    printf("\n=== SHIP STATUS ===\n");
    printf("Fuel: %d/%d\n", game->ship.fuel, game->ship.maxFuel);
    printf("Health: %d/%d\n", game->ship.health, game->ship.maxHealth);
    printf("Score: %d\n", game->score);
    /* Show inventory counts */
    printf("\n=== INVENTORY ===\n");
    printf("Metal: %d\n", game->ship.metal);
    printf("Plastic: %d\n", game->ship.plastic);
    printf("Electronics: %d\n", game->ship.electronics);
    printf("Fuel Cells: %d\n", game->ship.fuelCells);
    
    /* Show score needed to win based on difficulty level */
    printf("\nScore needed to win: %d\n", WIN_SCORES[game->difficulty]);
    /* Wait for player to acknowledge before continuing */
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

/* Display the welcome screen and game introduction */
void displayWelcomeMessage() {
    /* Show game title banner */
    printf("\n========================================\n");
    printf("        WELCOME TO SPACEXPLORER        \n");
    printf("========================================\n\n");
    
    /* Try to read introduction text from file */
    FILE* file = fopen(INTRO_FILE, "r");
    if (file != NULL) {
        /* Read and display intro text line by line */
        char line[100];
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
        }
        fclose(file);
    } else {
        /* If intro file doesn't exist, use default text and create the file */
        printf("You are an intrepid space explorer lost in deep space.\n");
        printf("Your mission is to collect space junk, avoid the dangerous asteroid,\n");
        printf("and find your way back home.\n\n");
        printf("Collect enough resources to win, but be careful of your fuel supply!\n");

        file = fopen(INTRO_FILE, "w");
        if (file != NULL) {
            /* Save default intro text to file for future use */
            fprintf(file, "You are an intrepid space explorer lost in deep space.\n");
            fprintf(file, "Your mission is to collect space junk, avoid the dangerous asteroid,\n");
            fprintf(file, "and find your way back home.\n\n");
            fprintf(file, "Collect enough resources to win, but be careful of your fuel supply!\n");
            fclose(file);
        }
    }
    /* Wait for player to start game */
    printf("\nPress Enter to start your adventure...");
    getchar();
}

/* Display end game message based on win/loss condition */
void displayEndGameMessage(Game* game) {
    /* Show end game banner */
    printf("\n========================================\n");
    
    if (game->hasWon) {
        /* Show win message */
        printf("             YOU WIN!                 \n");
        printf("========================================\n\n");
        printf("Congratulations, %s!\n", game->playerName);
        printf("You have collected enough resources and found your way home!\n");
        printf("Final Score: %d\n", game->score);
    } else {
        /* Show game over message */
        printf("             GAME OVER                \n");
        printf("========================================\n\n");
        
        /* Show reason for game over (fuel depletion or asteroid collision) */
        if (game->ship.fuel <= 0) {
            printf("Your spaceship ran out of fuel and is now drifting forever in space.\n");
        } else {
            printf("Your spaceship was hit by the asteroid and was destroyed.\n");
        }
        
        printf("Final Score: %d\n", game->score);
    }
    
    /* Save player's score to leaderboard */
    saveScore(game);
    
    /* Display the updated leaderboard */
    displayLeaderboard();
    
    /* Wait for player to exit */
    printf("\nPress Enter to exit...");
    getchar();
}

/* Save player's score to the leaderboard if it qualifies */
void saveScore(Game* game) {
    /* Create array to store leaderboard entries */
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int count = 0;
    
    /* Load existing leaderboard data */
    loadLeaderboard(leaderboard, &count);
    
    /* Check if the player's score qualifies for the leaderboard */
    if (count < MAX_LEADERBOARD_ENTRIES || game->score > leaderboard[count - 1].score) {
        /* Create new entry with player's data */
        LeaderboardEntry newEntry;
        strcpy(newEntry.playerName, game->playerName);
        newEntry.score = game->score;
        newEntry.difficulty = game->difficulty;
        
        /* Find insertion position in the sorted leaderboard */
        int insertIndex = count;
        
        /* Increment count if leaderboard isn't full */
        if (count < MAX_LEADERBOARD_ENTRIES) {
            count++;
        }
        
        /* Find the correct position to insert the new score (sorted by score) */
        for (int i = 0; i < count; i++) {
            if (newEntry.score > leaderboard[i].score) {
                insertIndex = i;
                break;
            }
        }
        
        /* Shift existing entries down to make room for new entry */
        for (int i = count - 1; i > insertIndex; i--) {
            leaderboard[i] = leaderboard[i - 1];
        }
        
        /* Insert the new entry at the correct position */
        leaderboard[insertIndex] = newEntry;
        
        /* Ensure leaderboard doesn't exceed maximum size */
        if (count > MAX_LEADERBOARD_ENTRIES) {
            count = MAX_LEADERBOARD_ENTRIES;
        }
        /* Save updated leaderboard to file */
        saveLeaderboard(leaderboard, count);
    }
}

/* Load the leaderboard data from file */
void loadLeaderboard(LeaderboardEntry leaderboard[], int* count) {
    /* Try to open leaderboard file */
    FILE* file = fopen(LEADERBOARD_FILE, "r");
    *count = 0;
    
    if (file != NULL) {
        /* Read entries until end of file or maximum entries is reached */
        while (*count < MAX_LEADERBOARD_ENTRIES && 
               fscanf(file, "%19[^,],%d,%d\n", 
                     leaderboard[*count].playerName,
                     &leaderboard[*count].score,
                     (int*)&leaderboard[*count].difficulty) == 3) {
            (*count)++;
        }
        
        fclose(file);
    }
}

/* Save the leaderboard data to file */
void saveLeaderboard(LeaderboardEntry leaderboard[], int count) {
    /* Open leaderboard file for writing */
    FILE* file = fopen(LEADERBOARD_FILE, "w");
    
    if (file != NULL) {
        /* Write each entry to the file in CSV format */
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s,%d,%d\n", 
                   leaderboard[i].playerName,
                   leaderboard[i].score,
                   leaderboard[i].difficulty);
        }
        
        fclose(file);
    }
}

/* Display the current leaderboard */
void displayLeaderboard() {
    /* Create array to store leaderboard entries */
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int count = 0;
    loadLeaderboard(leaderboard, &count);
    
    if (count > 0) {
        /* Display leaderboard header */
        printf("\n============ LEADERBOARD ============\n");
        printf("Rank | Name          | Score | Difficulty\n");
        printf("-------------------------------------\n");
        
        /* Display each leaderboard entry with formatting */
        for (int i = 0; i < count; i++) {
            char diffChar;
            /* Convert difficulty enum to character representation */
            switch (leaderboard[i].difficulty) {
                case EASY: diffChar = 'E'; break;
                case MEDIUM: diffChar = 'M'; break;
                case HARD: diffChar = 'H'; break;
                default: diffChar = '?'; break;
            }
            
            /* Print entry with formatting */
            printf("%-4d | %-14s | %-5d | %c\n", 
                   i + 1, 
                   leaderboard[i].playerName, 
                   leaderboard[i].score,
                   diffChar);
        }
    } else {
        /* Display message if leaderboard is empty */
        printf("\nNo high scores yet.\n");
    }
}

/* Placeholder function for future game update logic */
void updateGame(Game* game) {
    /* This function is currently empty but could be used
       for periodic updates like time-based events or animations */
} 