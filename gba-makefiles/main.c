//
// Created by jshartar6 on 11/9/17.
//
#include "myLib.h"
#include "ship.h"
#include "enemy.h"
#include "bullet.h"
#include "enemyBullet.h"
#include "kamikaze.h"
#include "startScreen.h"
#include "gameOver.h"
#include "gg.h"
#include "win.h"
#include "boss.h"

#define PROJECTILES 30
#define RIGHTBOUND 239
#define BOTTOMBOUND 142

typedef enum {EnemyBullet, Bullet, Enemy, Player, Kamikaze, Boss} GameObject;
struct Obj {
    GameObject ob;
    int x;
    int y;
    int width;
    int height;
    const u16* image;
    int health;
};

typedef enum {One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten} Level;
typedef enum {Start, Game, Win, GG, GameOver} State;

void drawGameObject(struct Obj item);
int shoot(struct Obj item);
int removeGameObject(struct Obj *item, int i, int numObjs);
int bound(int x, int y, int width, int height, int button);
int detectCol(struct Obj a, struct Obj b);
int collision(int a, int b, int width, int height, int c, int d, int wid, int hei);
int corner(int x, int y, int width, int height, int a, int b);
int enemyMove(struct Obj enemy);
void kamikazeMove(struct Obj *enemy, struct Obj player);
void bossMove(struct Obj *enemy, struct Obj player);
void levelOne(struct Obj *enemies, int *numEnemies);
void levelTwo(struct Obj *enemies, int *numEnemies);
void levelThree(struct Obj *enemies, int *numEnemies);
void levelFour(struct Obj *enemies, int *numEnemies);
void levelFive(struct Obj *enemies, int *numEnemies);
void levelSix(struct Obj *enemies, int *numEnemies);
void levelSeven(struct Obj *enemies, int *numEnemies);
void levelEight(struct Obj *enemies, int *numEnemies);
void levelNine(struct Obj *enemies, int *numEnemies);
void levelTen(struct Obj *enemies, int *numEnemies);
int game();
struct Obj enemyShoot(struct Obj enemy);
struct Obj bossShootNormal(struct Obj enemy);
struct Obj bossShootSpecial(struct Obj enemy);
int lives;

// main
int main(void) {
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    int draw;
    int pressing;
    int ggpress;
    int winpress;
    winpress =FALSE;
    ggpress = FALSE;
    draw = TRUE;
    pressing = FALSE;
    State state;
    state = Start;
    int next;

    while(1) {
        switch(state) {
            case Start :
                if (draw) {
                    drawImage3(0, 0, STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, startScreen);
                    draw = FALSE;
                }
                if (KEY_DOWN_NOW(BUTTON_START)) {
                    pressing = TRUE;
                } else if (pressing) {
                    drawRectangle(0, 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, BLACK);
                    pressing = FALSE;
                    state = Game;
                    draw = TRUE;
                }
                break;
            case Game :
                next = game();
                if (next == 0) {
                    state = Start;
                } else if (next == 1) {
                    state = Win;
                } else {
                    state = GG;
                }
                break;
            case Win :
                if (draw) {
                    for (int i = 0; i < 13; i++) {
                        drawImage3(0, 0, WIN_WIDTH, WIN_HEIGHT, win_frames[i]);
                    }
                }
                if (KEY_DOWN_NOW(BUTTON_START)) {
                    winpress = TRUE;
                } else if (winpress) {
                    drawRectangle(0 , 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, BLACK);
                    pressing = FALSE;
                    state = GameOver;
                    draw = TRUE;
                }
                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    pressing = TRUE;
                } else if (pressing) {
                    drawRectangle(0 , 0, STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, BLACK);
                    pressing = FALSE;
                    state = Start;
                    draw = TRUE;
                }
                break;
            case GG :
                if (draw) {
                    drawImage3(0, 0, GG_WIDTH, GG_HEIGHT, gg);
                }
                if (KEY_DOWN_NOW(BUTTON_START)) {
                    ggpress = TRUE;
                } else if (ggpress) {
                    drawRectangle(0 , 0, STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, BLACK);
                    pressing = FALSE;
                    state = GameOver;
                    draw = TRUE;
                }
                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    pressing = TRUE;
                } else if (pressing) {
                    drawRectangle(0 , 0, STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, BLACK);
                    pressing = FALSE;
                    state = Start;
                    draw = TRUE;
                }
                break;
            case GameOver :
                if (draw) {
                    drawImage3(0, 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, gameOver);
                    draw = FALSE;
                }
                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    pressing = TRUE;
                } else if (pressing) {
                    drawRectangle(0 , 0, STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, BLACK);
                    pressing = FALSE;
                    state = Start;
                    draw = TRUE;
                }
                break;
        }
    }
    return 0;
}
// runs the game
int game() {
    REG_DISPCNT = MODE_3 | BG2_ENABLE;

    lives = 3;
    struct Obj projectiles[PROJECTILES];
    int numProjectiles;
    numProjectiles = 0;
    struct Obj enemies[55];
    int numEnemies;
    numEnemies = 0;

    struct Obj player;
    player.x = (120 - (SHIP_WIDTH / 2));
    player.y = (BOTTOMBOUND - (SHIP_HEIGHT));
    player.width = SHIP_WIDTH;
    player.height = SHIP_HEIGHT;
    player.image = ship;
    player.ob = Player;
    player.health = 1;
    drawGameObject(player);

    drawBlankRectangle(0, 0, RIGHTBOUND, BOTTOMBOUND, 31);

    levelOne(enemies, &numEnemies);

    drawImage3(1, BOTTOMBOUND + 1, SHIP_WIDTH, SHIP_HEIGHT, ship);
    drawImage3(17, BOTTOMBOUND + 1, SHIP_WIDTH, SHIP_HEIGHT, ship);

    int a;
    a = FALSE;
    int gameClock;
    gameClock = 1;
    Level level = One;
    int setLevel;
    setLevel = TRUE;

    while (1) {
        if (KEY_DOWN_NOW(BUTTON_SELECT)) {
            return 0;
        }
        //bullet stuff
        for (int i = 0; i < numProjectiles; i++) {
            if (!bound(projectiles[i].x, projectiles[i].y,projectiles[i].width, projectiles[i].height, BUTTON_UP)
                    || !bound(projectiles[i].x, projectiles[i].y,projectiles[i].width, projectiles[i].height, BUTTON_DOWN)) {
                numProjectiles = removeGameObject(projectiles, i, numProjectiles);
            } else if (numProjectiles > 0) {
                projectiles[i].y = shoot(projectiles[i]);
                for (int j = 0; j < numEnemies; j++) {
                    if (projectiles[i].ob == Bullet && detectCol(projectiles[i], enemies[j])) {
                        if (enemies[j].ob == Boss) {
                            enemies[j].health = enemies[j].health - 1;
                            numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                            if (enemies[j].health == 0) {
                                numEnemies = removeGameObject(enemies, j, numEnemies);
                            }
                        } else {
                            numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                            numEnemies = removeGameObject(enemies, j, numEnemies);
                        }
                    }
                }
                if (projectiles[i].ob == EnemyBullet && detectCol(projectiles[i], player)) {
                    numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    lives--;
                    if (lives == 2) {
                        drawRectangle(17, BOTTOMBOUND + 1, SHIP_WIDTH, SHIP_HEIGHT, BLACK);
                    } else if (lives == 1) {
                        drawRectangle(1, BOTTOMBOUND + 1, SHIP_WIDTH, SHIP_HEIGHT, BLACK);
                    }

                }
            }
        }

        //enemy movement
        if (gameClock % 100 == 0) {
            for (int i = 0; i < numEnemies; i++) {
                if (enemies[i].ob == Enemy) {
                    enemies[i].y = enemyMove(enemies[i]);
                    if ((player.x - 15) <= enemies[i].x && enemies[i].x <= (player.x + 15)) {
                        projectiles[numProjectiles] = enemyShoot(enemies[i]);
                        numProjectiles++;
                    }
                    waitForV();
                }
            }
        }

        //kamikaze movement, if they miss and hit bottom of screen they die
        if (gameClock % 5 == 0) {
            for (int i = 0; i < numEnemies; i++) {
                if (enemies[i].ob == Kamikaze) {
                    kamikazeMove(&(enemies[i]), player);
                    if (enemies[i].y == player.y) {
                        removeGameObject(enemies, i, numEnemies);
                    }
                    waitForV();
                }
            }
        }

        //boss movement
        if (gameClock % 5 == 0) {
            for (int i = 0; i < numEnemies; i++) {
                if (enemies[i].ob == Boss) {
                    bossMove(&(enemies[i]), player);
                    if (gameClock % 200 == 0) {
                        enemies[numEnemies] = bossShootSpecial(enemies[i]);
                        numEnemies++;
                    }
                    if ((player.x - 15) <= enemies[i].x && enemies[i].x <= (player.x + 15)) {
                        projectiles[numProjectiles] = bossShootNormal(enemies[i]);
                        numProjectiles++;
                    }
                    waitForV();
                }
            }
        }

        //collision handling, enemy hit player both die
        for (int i = 0; i < numEnemies; i++) {
            if (detectCol(enemies[i], player)) {
                numEnemies = removeGameObject(enemies, i, numEnemies);
                lives--;
                if (lives == 2) {
                    drawRectangle(17, BOTTOMBOUND + 1, SHIP_WIDTH, SHIP_HEIGHT, BLACK);
                } else if (lives == 1) {
                    drawRectangle(1, BOTTOMBOUND + 1, SHIP_WIDTH, SHIP_HEIGHT, BLACK);
                }
            }
        }
        //player movement
        if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
            if (bound(player.x, player.y, player.width, player.height, BUTTON_RIGHT)) {
                drawRectangle(player.x, player.y, player.width, player.height, BLACK);
                player.x = player.x + 1;
                drawGameObject(player);
            }
        }
        if (KEY_DOWN_NOW(BUTTON_LEFT)) {
            if (bound(player.x, player.y, player.width, player.height, BUTTON_LEFT)) {
                drawRectangle(player.x, player.y, player.width, player.height, BLACK);
                player.x = player.x - 1;
                drawGameObject(player);
            }
        }
        if (KEY_DOWN_NOW(BUTTON_UP)) {
            if (bound(player.x, player.y, player.width, player.height, BUTTON_UP)) {
                drawRectangle(player.x, player.y, player.width, player.height, BLACK);
                player.y = player.y - 1;
                drawGameObject(player);
            }
        }
        if (KEY_DOWN_NOW(BUTTON_DOWN)) {
            if (bound(player.x, player.y, player.width, player.height, BUTTON_DOWN)) {
                drawRectangle(player.x, player.y, player.width, player.height, BLACK);
                player.y = player.y + 1;
                drawGameObject(player);
            }
        }
        // delay for when player can shoot
        if (gameClock % 3 == 0) {
            if(KEY_DOWN_NOW(BUTTON_A)) {
                a = TRUE;
            }
            if(!KEY_DOWN_NOW(BUTTON_A) && a) {
                a = FALSE;
                if (numProjectiles < (PROJECTILES - 3)) {
                    struct Obj shot;
                    shot.ob = Bullet;
                    shot.x = (player.x + (SHIP_WIDTH / 2) - (BULLET_WIDTH) + 1);
                    shot.y = (player.y - BULLET_HEIGHT);
                    shot.image = bullet;
                    shot.width = BULLET_WIDTH;
                    shot.height = BULLET_HEIGHT;
                    projectiles[numProjectiles] = shot;
                    numProjectiles++;
                }
            }
        }
        // gg/ game over
        if (lives == 0) {
            return 2;
        }
        // levels
        switch(level) {
            case One :
                if (numEnemies == 0) {
                    level = Two;
                    setLevel = TRUE;
                }
                break;
            case Two :
                if (setLevel) {
                    for (int i = numProjectiles - 1; i >= 0; i--) {
                        numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    }
                    levelTwo(enemies, &numEnemies);
                    setLevel = FALSE;
                } else if (numEnemies == 0) {
                    level = Three;
                    setLevel = TRUE;
                }
                break;
            case Three :
                if (setLevel) {
                    for (int i = numProjectiles - 1; i >= 0; i--) {
                        numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    }
                    levelThree(enemies, &numEnemies);
                    setLevel = FALSE;
                } else if (numEnemies == 0) {
                    level = Four;
                    setLevel = TRUE;
                }
                break;
            case Four :
                if (setLevel) {
                    for (int i = numProjectiles - 1; i >= 0; i--) {
                        numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    }
                    levelFour(enemies, &numEnemies);
                    setLevel = FALSE;
                } else if (numEnemies == 0) {
                    level = Five;
                    setLevel = TRUE;
                }
                break;
            case Five :
                if (setLevel) {
                    for (int i = numProjectiles - 1; i >= 0; i--) {
                        numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    }
                    levelFive(enemies, &numEnemies);
                    setLevel = FALSE;
                } else if (numEnemies == 0) {
                    level = Six;
                    setLevel = TRUE;
                }
                break;
            case Six :
                if (setLevel) {
                    for (int i = numProjectiles - 1; i >= 0; i--) {
                        numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    }
                    levelSix(enemies, &numEnemies);
                    setLevel = FALSE;
                } else if (numEnemies == 0) {
                    level = Seven;
                    setLevel = TRUE;
                }
                break;
            case Seven :
                if (setLevel) {
                    for (int i = numProjectiles - 1; i >= 0; i--) {
                        numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    }
                    levelSeven(enemies, &numEnemies);
                    setLevel = FALSE;
                } else if (numEnemies == 0) {
                    level = Eight;
                    setLevel = TRUE;
                }
                break;
            case Eight :
                if (setLevel) {
                    for (int i = numProjectiles - 1; i >= 0; i--) {
                        numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    }
                    levelEight(enemies, &numEnemies);
                    setLevel = FALSE;
                } else if (numEnemies == 0) {
                    level = Nine;
                    setLevel = TRUE;
                }
                break;
            case Nine :
                if (setLevel) {
                    for (int i = numProjectiles - 1; i >= 0; i--) {
                        numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    }
                    levelNine(enemies, &numEnemies);
                    setLevel = FALSE;
                } else if (numEnemies == 0) {
                    level = Ten;
                    setLevel = TRUE;
                }
                break;
            case Ten :
                if (setLevel) {
                    for (int i = numProjectiles - 1; i >= 0; i--) {
                        numProjectiles = removeGameObject(projectiles, i, numProjectiles);
                    }
                    levelTen(enemies, &numEnemies);
                    setLevel = FALSE;
                } else if (numEnemies == 0) {
                    return 1;
                }
                break;

        }
        waitForV();
        gameClock++;
    }
    return 1;
}

//draw game objects
void drawGameObject(struct Obj item) {
    drawImage3(item.x, item.y, item.width, item.height, item.image);
}
// shoot function
int shoot(struct Obj item) {
    if (item.ob == Bullet) {
        drawRectangle(item.x, item.y, BULLET_WIDTH, BULLET_HEIGHT, BLACK);
        drawImage3(item.x, item.y - 1, BULLET_WIDTH, BULLET_HEIGHT, bullet);
        return (item.y - 1);
    } else {
        drawRectangle(item.x, item.y, ENEMYBULLET_WIDTH, ENEMYBULLET_HEIGHT, BLACK);
        drawImage3(item.x, item.y + 1, ENEMYBULLET_WIDTH, ENEMYBULLET_HEIGHT, enemyBullet);
        return (item.y + 1);
    }
}
// boss shooting stuff
struct Obj bossShootNormal(struct Obj enemy) {
    struct Obj enemyShot;
    enemyShot.x = (enemy.x + (enemy.width / 2) - (ENEMYBULLET_WIDTH / 2));
    enemyShot.y = (enemy.y + enemy.height);
    enemyShot.width = ENEMYBULLET_WIDTH;
    enemyShot.height = ENEMYBULLET_HEIGHT;
    enemyShot.image = enemyBullet;
    enemyShot.ob = EnemyBullet;
    drawGameObject(enemyShot);
    return enemyShot;
}

struct Obj bossShootSpecial(struct Obj enemy) {
    struct Obj enemyShot;
    enemyShot.x = (enemy.x + (enemy.width / 2) - (KAMIKAZE_WIDTH / 2));
    enemyShot.y = (enemy.y + enemy.height);
    enemyShot.width = KAMIKAZE_WIDTH;
    enemyShot.height = KAMIKAZE_HEIGHT;
    enemyShot.image = kamikaze;
    enemyShot.ob = Kamikaze;
    drawGameObject(enemyShot);
    return enemyShot;
}

//enemy shoot function
struct Obj enemyShoot(struct Obj enemy) {
    struct Obj enemyShot;
    enemyShot.x = (enemy.x + (enemy.width / 2) - (ENEMYBULLET_WIDTH / 2));
    enemyShot.y = (enemy.y + enemy.height);
    enemyShot.width = ENEMYBULLET_WIDTH;
    enemyShot.height = ENEMYBULLET_HEIGHT;
    enemyShot.image = enemyBullet;
    enemyShot.ob = EnemyBullet;
    drawGameObject(enemyShot);
    return enemyShot;
}
// removing objects
int removeGameObject(struct Obj *item, int i, int numObjs) {
    drawRectangle(item[i].x, item[i].y, item[i].width, item[i].height, BLACK);
    item[i] = item[numObjs - 1];
    return (numObjs - 1);
}
// boundry of objects
int bound(int x, int y, int width, int height, int button) {
    if ((button & BUTTON_RIGHT) && (x + width + 1) >= RIGHTBOUND) {
        return 0;
    } else if ((button & BUTTON_LEFT) && (x -1) <= 0) {
        return 0;
    } else if ((button & BUTTON_UP) && (y - 1) <= 0) {
        return 0;
    } else if ((button & BUTTON_DOWN) && (y + height + 1) >= BOTTOMBOUND) {
        return 0;
    } else {
        return 1;
    }
}
// detect when hit
int detectCol(struct Obj a, struct Obj b) {
    return (collision(a.x, a.y, a.width, a.height, b.x, b.y, b.width, b.height)) || (collision(b.x, b.y, b.width, b.height, a.x, a.y, a.width, a.height));
}
// determine if hit
int collision(int a, int b, int width, int height, int c, int d, int wid, int hei) {
    if (corner(a, b, width, height, c, d)) {
        return 1;
    } else if (corner(a, b, width, height, (c + wid), d)) {
        return 1;
    } else if (corner(a, b, width, height,(c + wid), (d + hei))) {
        return 1;
    } else if (corner(a, b, width, height, c, (d + hei))) {
        return 1;
    } else {
        return 0;
    }
}
// corners
int corner(int x, int y, int width, int height, int a, int b) {
    return (x <= a && a <= (x + width) && y <= b && b <= (y + height));
}
// enemy movement
int enemyMove(struct Obj enemy) {
    drawRectangle(enemy.x, enemy.y, enemy.width, enemy.height, BLACK);
    drawImage3(enemy.x, enemy.y + 1, enemy.width, enemy.height, enemy.image);
    return enemy.y + 1;
}
// kamikaze movement
void kamikazeMove(struct Obj *enemy, struct Obj player) {
    drawRectangle(enemy->x, enemy->y, enemy->width, enemy->height, BLACK);
    if (enemy->x < player.x) {
        enemy->x = enemy->x + 1;
    } else if (enemy->x > player.x) {
        enemy->x = enemy->x - 1;
    }
    if (enemy->y < player.y) {
        enemy->y = enemy->y + 1;
    } else if (enemy->y > player.y) {
        enemy->y = enemy->y - 1;
    }
    drawGameObject(*enemy);
}
// boss movement
void bossMove(struct Obj *enemy, struct Obj player) {
    drawRectangle(enemy->x, enemy->y, enemy->width, enemy->height, BLACK);
    if (enemy->x < player.x) {
        enemy->x = enemy->x + 1;
    } else if (enemy->x > player.x) {
        enemy->x = enemy->x - 1;
    }
    drawGameObject(*enemy);
}
// level set up for enemies
void levelOne(struct Obj *enemies, int *numEnemies) {
    int size;
    size = KAMIKAZE_WIDTH;
    for (int i = 0; i < 4; i++) {
        struct Obj k;
        k.x = size;
        k.y = 2;
        k.width = KAMIKAZE_WIDTH;
        k.height = KAMIKAZE_HEIGHT;
        k.image = kamikaze;
        k.ob = Kamikaze;
        k.health = 1;
        drawGameObject(k);
        enemies[*numEnemies] = k;
        size += KAMIKAZE_WIDTH * 5;
        *numEnemies = *numEnemies + 1;
    }
}

void levelTwo(struct Obj *enemies, int *numEnemies) {
    int size;
    size = 3;
    for (int i = 0; i < 12; i++) {
        struct Obj en;
        en.x = size;
        en.y = 1;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + (ENEMY_WIDTH / 2) + 1;
        *numEnemies = *numEnemies + 1;
    }
}

void levelThree(struct Obj *enemies, int *numEnemies) {
    int size;
    size = 1;
    for (int i = 0; i < 17; i++) {
        struct Obj en;
        en.x = size;
        en.y = 1;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + 1;
        *numEnemies = *numEnemies + 1;
    }
}

void levelFour(struct Obj *enemies, int *numEnemies) {
    int size;
    size = (KAMIKAZE_WIDTH * 3) / 2;
    int change;
    change = 2;
    for (int i = 0; i < 7; i++) {
        if (i < 4) {
            change += 8;
        } else {
            change -= 8;
        }
        struct Obj k;
        k.x = size;
        k.y = change;
        k.width = KAMIKAZE_WIDTH;
        k.height = KAMIKAZE_HEIGHT;
        k.image = kamikaze;
        k.ob = Kamikaze;
        k.health = 1;
        drawGameObject(k);
        enemies[*numEnemies] = k;
        size += (KAMIKAZE_WIDTH * 5) / 2;
        *numEnemies = *numEnemies + 1;
    }
}

void levelFive(struct Obj *enemies, int *numEnemies) {
    int size;
    size = 6 + ((ENEMY_WIDTH / 3) *2);
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = 1;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = 4;
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = ENEMY_HEIGHT + 2;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }
}
void levelSix(struct Obj *enemies, int *numEnemies) {
    int size;
    size = 6 + ((ENEMY_WIDTH / 3) *2);
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = 1;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = 4;
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = ENEMY_HEIGHT + 2;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = KAMIKAZE_WIDTH;
    for (int i = 0; i < 4; i++) {
        struct Obj k;
        k.x = size;
        k.y = ENEMY_HEIGHT * 2 + 2;
        k.width = KAMIKAZE_WIDTH;
        k.height = KAMIKAZE_HEIGHT;
        k.image = kamikaze;
        k.ob = Kamikaze;
        k.health = 1;
        drawGameObject(k);
        enemies[*numEnemies] = k;
        size += KAMIKAZE_WIDTH * 5;
        *numEnemies = *numEnemies + 1;
    }
}

void levelSeven(struct Obj *enemies, int *numEnemies) {
    int size;
    size = 6 + ((ENEMY_WIDTH / 3) *2);
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = 1;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = 4;
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = ENEMY_HEIGHT + 2;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = 6 + ((ENEMY_WIDTH / 3) *2);
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = ENEMY_HEIGHT * 2 + 2;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = 4;
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = ENEMY_HEIGHT * 3 + 7;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }
}

void levelEight(struct Obj *enemies, int *numEnemies) {
    int size;
    size = (KAMIKAZE_WIDTH * 3) / 2;
    int change;
    change = 2;
    for (int i = 0; i < 8; i++) {
        if (i < 4) {
            change += 8;
        } else {
            change -= 8;
        }
        struct Obj k;
        k.x = size;
        k.y = change;
        k.width = KAMIKAZE_WIDTH;
        k.height = KAMIKAZE_HEIGHT;
        k.image = kamikaze;
        k.ob = Kamikaze;
        k.health = 1;
        drawGameObject(k);
        enemies[*numEnemies] = k;
        size += (KAMIKAZE_WIDTH * 5) / 2;
        *numEnemies = *numEnemies + 1;
    }

    size = 2 + (KAMIKAZE_WIDTH * 3) / 2;
    change = 2;
    for (int i = 0; i < 8; i++) {
        if (i < 4) {
            change += 8;
        } else {
            change -= 8;
        }
        struct Obj k;
        k.x = size;
        k.y = KAMIKAZE_HEIGHT + change + 2;
        k.width = KAMIKAZE_WIDTH;
        k.height = KAMIKAZE_HEIGHT;
        k.image = kamikaze;
        k.ob = Kamikaze;
        k.health = 1;
        drawGameObject(k);
        enemies[*numEnemies] = k;
        size += (KAMIKAZE_WIDTH * 5) / 2;
        *numEnemies = *numEnemies + 1;
    }
}

void levelNine(struct Obj *enemies, int *numEnemies) {
    int size;
    size = 6 + ((ENEMY_WIDTH / 3) *2);
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = 1;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = 4;
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = ENEMY_HEIGHT + 2;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = 6 + ((ENEMY_WIDTH / 3) *2);
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = ENEMY_HEIGHT * 2 + 2;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = 4;
    for (int i = 0; i < 11; i++) {
        struct Obj en;
        en.x = size;
        en.y = ENEMY_HEIGHT * 3 + 2;
        en.width = ENEMY_WIDTH;
        en.height = ENEMY_HEIGHT;
        en.image = enemy;
        en.ob = Enemy;
        en.health = 1;
        drawGameObject(en);
        enemies[*numEnemies] = en;
        size += ENEMY_WIDTH + ((ENEMY_WIDTH / 3) * 2);
        *numEnemies = *numEnemies + 1;
    }

    size = 2 + (KAMIKAZE_WIDTH * 3) / 2;
    int change;
    change = 2;
    for (int i = 0; i < 8; i++) {
        if (i < 4) {
            change += 8;
        } else {
            change -= 8;
        }
        struct Obj k;
        k.x = size;
        k.y = ENEMY_HEIGHT * 5 + change + 2;
        k.width = KAMIKAZE_WIDTH;
        k.height = KAMIKAZE_HEIGHT;
        k.image = kamikaze;
        k.ob = Kamikaze;
        k.health = 1;
        drawGameObject(k);
        enemies[*numEnemies] = k;
        size += (KAMIKAZE_WIDTH * 5) / 2;
        *numEnemies = *numEnemies + 1;
    }

}

void levelTen(struct Obj *enemies, int *numEnemies) {
    int size;
    size = 8 + (BOSS_WIDTH / 3) * 2;
    for (int i = 0; i < 2; i++) {
        struct Obj b;
        b.x = size;
        b.y = BOSS_HEIGHT - 10;
        b.width = BOSS_WIDTH;
        b.height = BOSS_HEIGHT;
        b.ob = Boss;
        b.image = boss;
        b.health = 15;
        drawGameObject(b);
        enemies[*numEnemies] = b;
        size += (BOSS_WIDTH * 4) / 2;
        *numEnemies = *numEnemies + 1;
    }
}







