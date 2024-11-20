#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_PLAYERS 4
#define MAX_HEALTH 100
#define WIN_PROGRESS 30

typedef struct {
    char name[50];
    int health;
    int attack;
    int defense;
    int progress; 
} Player;

typedef struct {
    char name[50];
    int health;
    int attack;
    int defense;
} Enemy;

int rollDice(int sides);
Enemy createEnemy();
void encounterEnemy(Player *player);
void displayPlayerStats(Player *player);
void playerTurn(Player *player);
void gameLoop(Player players[], int numPlayers);

int main() {
    srand(time(NULL));

    int numPlayers;
    printf("Welcome to Dungeons & Dragons: CLI Edition!\n");
    printf("Enter the number of players (1 to %d): ", MAX_PLAYERS);
    scanf("%d", &numPlayers);
    getchar(); 

    if (numPlayers < 1 || numPlayers > MAX_PLAYERS) {
        printf("Invalid number of players. Exiting game.\n");
        return 1;
    }

    Player players[MAX_PLAYERS];
    for (int i = 0; i < numPlayers; i++) {
        printf("Enter name for Player %d: ", i + 1);
        fgets(players[i].name, sizeof(players[i].name), stdin);
        players[i].name[strcspn(players[i].name, "\n")] = '\0'; 
        players[i].health = MAX_HEALTH;
        players[i].attack = 10 + (rand() % 5);
        players[i].defense = 5 + (rand() % 5);
        players[i].progress = 0;
    }

    gameLoop(players, numPlayers);

    return 0;
}

int rollDice(int sides) {
    return (rand() % sides) + 1;
}

Enemy createEnemy() {
    Enemy enemy;
    if (rand() % 2 == 0) {
        strcpy(enemy.name, "Goblin");
        enemy.health = 30 + (rand() % 20);
        enemy.attack = 8 + (rand() % 5);
        enemy.defense = 3 + (rand() % 3);
    } else {
        strcpy(enemy.name, "Orc");
        enemy.health = 50 + (rand() % 30);
        enemy.attack = 12 + (rand() % 5);
        enemy.defense = 5 + (rand() % 3);
    }
    return enemy;
}

void encounterEnemy(Player *player) {
    Enemy enemy = createEnemy();
    printf("\n--- %s encounters a %s! ---\n", player->name, enemy.name);
    printf("%s's Health: %d | Attack: %d | Defense: %d\n", enemy.name, enemy.health, enemy.attack, enemy.defense);

    while (player->health > 0 && enemy.health > 0) {
        printf("\n%s's turn against the %s. Choose an action:\n", player->name, enemy.name);
        printf("1. Attack\n");
        printf("2. Defend\n");
        printf("Enter your choice: ");

        int choice;
        scanf("%d", &choice);

        if (choice == 1) { 
            int damage = player->attack - enemy.defense;
            if (damage < 0) damage = 0;
            enemy.health -= damage;
            printf("%s attacks the %s, dealing %d damage! %s's remaining health: %d\n",
                   player->name, enemy.name, damage, enemy.name, (enemy.health > 0 ? enemy.health : 0));
        } else if (choice == 2) { 
            printf("%s braces for an attack, boosting their defense!\n", player->name);
            player->defense += 3; 
        } else {
            printf("Invalid choice! You hesitate and lose your chance to act.\n");
        }

        if (enemy.health > 0) {
            int damage = enemy.attack - player->defense;
            if (damage < 0) damage = 0;
            player->health -= damage;
            printf("The %s attacks %s, dealing %d damage! %s's remaining health: %d\n",
                   enemy.name, player->name, damage, player->name, (player->health > 0 ? player->health : 0));
        }
    }

    if (player->health <= 0) {
        printf("%s is defeated by the %s and can no longer continue.\n", player->name, enemy.name);
    } else if (enemy.health <= 0) {
        printf("The %s is defeated! %s can move forward.\n", enemy.name, player->name);
    }
}

void displayPlayerStats(Player *player) {
    printf("\n--- %s's Stats ---\n", player->name);
    printf("Health: %d\n", player->health);
    printf("Attack: %d\n", player->attack);
    printf("Defense: %d\n", player->defense);
    printf("Dungeon Progress: %d steps\n", player->progress);
}

void playerTurn(Player *player) {
    if (player->health <= 0) {
        printf("%s is knocked out and cannot take a turn.\n", player->name);
        return;
    }

    displayPlayerStats(player);

    printf("\n%s's turn! Choose an action:\n", player->name);
    printf("1. Roll Dice and Move\n");
    printf("2. Heal (+10 health)\n");
    printf("3. Rest (Skip turn to recover defenses)\n");
    printf("Choose your action: ");

    int choice;
    scanf("%d", &choice);

    switch (choice) {
        case 1: {
            int diceRoll = rollDice(6);
            printf("%s rolls a %d and moves forward!\n", player->name, diceRoll);
            player->progress += diceRoll;

            if (rand() % 2 == 0) { 
                encounterEnemy(player);
            } else {
                printf("The path is clear. No enemies encountered.\n");
            }
            break;
        }
        case 2:
            player->health += 10;
            if (player->health > MAX_HEALTH) player->health = MAX_HEALTH;
            printf("%s heals and now has %d health.\n", player->name, player->health);
            break;
        case 3:
            printf("%s rests and recovers their strength.\n", player->name);
            player->defense += 2;
            break;
        default:
            printf("Invalid choice. Skipping turn.\n");
            break;
    }
}

void gameLoop(Player players[], int numPlayers) {
    int gameOver = 0;

    while (!gameOver) {
        for (int i = 0; i < numPlayers; i++) {
            if (players[i].health > 0) {
                playerTurn(&players[i]);

                if (players[i].progress >= WIN_PROGRESS) {
                    printf("\n%s has reached the end of the dungeon and won the game!\n", players[i].name);
                    gameOver = 1;
                    break;
                }
            }
        }

        int alivePlayers = 0;
        for (int i = 0; i < numPlayers; i++) {
            if (players[i].health > 0) alivePlayers++;
        }
        if (alivePlayers == 0) {
            printf("\nAll players are defeated. Game Over!\n");
            gameOver = 1;
        }
    }
}
