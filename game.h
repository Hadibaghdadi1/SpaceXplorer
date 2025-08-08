/**
 * SpaceXplorer Game Header
 * 
 * This header defines all structures, enumerations, constants,
 * and function prototypes for the SpaceXplorer game.
 */

#ifndef SPACEXPLORER_GAME_H
#define SPACEXPLORER_GAME_H

/* Minimum world size in both dimensions */
#define WORLD_MIN_SIZE 18
/* Maximum player name length including null terminator */
#define MAX_NAME_LENGTH 20
/* Maximum number of collectible junk items in the world */
#define MAX_JUNK_ITEMS 40
/* Maximum number of entries in the high score leaderboard */
#define MAX_LEADERBOARD_ENTRIES 10
/* Number of impassable obstacle cells in the world */
#define IMPASSABLE_CELLS 3

/**
 * Game difficulty settings
 * - EASY: More fuel, slower asteroid, more junk, lower win score
 * - MEDIUM: Medium values for all parameters
 * - HARD: Less fuel, faster asteroid, less junk, higher win score
 */
typedef enum {
    EASY,
    MEDIUM,
    HARD
} Difficulty;

/**
 * 2D Position structure for game objects
 * Used to track location of the ship, asteroid, junk, and obstacles
 */
typedef struct {
    int x; /* X-coordinate (horizontal position) */
    int y; /* Y-coordinate (vertical position) */
} Position;

/**
 * Types of space junk that can be collected
 * Each type has different value and usage
 */
typedef enum {
    METAL,        /* Used to repair ship */
    PLASTIC,      /* Basic collectible */
    ELECTRONICS,  /* Valuable collectible */
    FUEL_CELL     /* Used to refuel ship */
} JunkType;

/**
 * Space junk item structure
 * Represents collectible items that appear in the world
 */
typedef struct {
    Position position;  /* Location in the world */
    JunkType type;      /* Type of junk item */
    int value;          /* Score value when collected */
    char symbol;        /* Character displayed on the map */
    int collected;      /* Flag indicating if already collected */
} SpaceJunk;

/**
 * Player's spaceship structure
 * Contains stats, position, and inventory
 */
typedef struct {
    Position position;   /* Current location in the world */
    int fuel;            /* Current fuel level */
    int maxFuel;         /* Maximum fuel capacity */
    int health;          /* Current health points */
    int maxHealth;       /* Maximum health points */
    int metal;           /* Count of metal pieces in inventory */
    int plastic;         /* Count of plastic pieces in inventory */
    int electronics;     /* Count of electronics in inventory */
    int fuelCells;       /* Count of fuel cells in inventory */
} Spaceship;

/**
 * Asteroid structure
 * Represents the dangerous moving obstacle
 */
typedef struct {
    Position position;   /* Current location in the world */
    Position direction;  /* Direction of movement (dx, dy) */
    char symbol;         /* Character displayed on the map */
} Asteroid;

/**
 * Impassable cell structure
 * Represents static obstacles in the world
 */
typedef struct {
    Position position;   /* Location in the world */
    char symbol;         /* Character displayed on the map */
} ImpassableCell;

/**
 * Main game structure
 * Contains all game state information
 */
typedef struct {
    int worldWidth;                              /* Width of the game world */
    int worldHeight;                             /* Height of the game world */
    char** world;                                /* 2D array representing the world */
    Spaceship ship;                              /* Player's spaceship */
    Asteroid asteroid;                           /* Moving asteroid obstacle */
    SpaceJunk junkItems[MAX_JUNK_ITEMS];         /* Array of collectible items */
    int junkCount;                               /* Actual number of junk items */
    ImpassableCell impassableCells[IMPASSABLE_CELLS]; /* Array of impassable obstacles */
    int score;                                   /* Player's current score */
    int isGameOver;                              /* Flag indicating if game has ended */
    int hasWon;                                  /* Flag indicating if player won */
    Difficulty difficulty;                       /* Current game difficulty */
    char playerName[MAX_NAME_LENGTH];            /* Player's name */
} Game;

/**
 * Leaderboard entry structure
 * For storing and displaying high scores
 */
typedef struct {
    char playerName[MAX_NAME_LENGTH];  /* Player's name */
    int score;                         /* Player's score */
    Difficulty difficulty;             /* Difficulty level achieved */
} LeaderboardEntry;

/* Function to initialize a new game with player name and difficulty settings */
void initGame(Game* game);
/* Load world size configuration from file */
void loadConfig(Game* game);
/* Save player's score to the leaderboard */
void saveScore(Game* game);
/* Load leaderboard data from file */
void loadLeaderboard(LeaderboardEntry leaderboard[], int* count);
/* Save leaderboard data to file */
void saveLeaderboard(LeaderboardEntry leaderboard[], int count);
/* Allocate memory for the game world */
void createWorld(Game* game);
/* Draw the game world and display status */
void renderWorld(Game* game);
/* Process player input commands */
void handleInput(Game* game);
/* Update game state (currently unused) */
void updateGame(Game* game);
/* Move player's spaceship */
void moveSpaceship(Game* game, int dx, int dy);
/* Move the asteroid obstacle */
void moveAsteroid(Game* game);
/* Check for collisions with junk items and win condition */
void checkCollisions(Game* game);
/* Process collection of a junk item */
void collectJunk(Game* game, int index);
/* Use collected items to repair ship or refuel */
void useJunk(Game* game, int option);
/* Display detailed ship status and inventory */
void displayShipStatus(Game* game);
/* Show welcome screen and game introduction */
void displayWelcomeMessage();
/* Show game over or victory screen */
void displayEndGameMessage(Game* game);
/* Display the high score leaderboard */
void displayLeaderboard();
/* Free allocated memory when game ends */
void cleanupGame(Game* game);

#endif /* SPACEXPLORER_GAME_H */