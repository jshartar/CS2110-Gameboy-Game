/*
 * Exported with nin10kit v1.7
 * Time-stamp: Thursday 11/09/2017, 18:36:13
 * 
 * Image Information
 * -----------------
 * /home/jshartar6/Desktop/CS2110/HW09/gba-makefiles/Kamikaze.png 13@10
 * /home/jshartar6/Desktop/CS2110/HW09/gba-makefiles/bullet.png 3@8
 * /home/jshartar6/Desktop/CS2110/HW09/gba-makefiles/enemy.gif 41@32
 * /home/jshartar6/Desktop/CS2110/HW09/gba-makefiles/enemyBullet.png 3@8
 * /home/jshartar6/Desktop/CS2110/HW09/gba-makefiles/gameover.jpg 320@227
 * /home/jshartar6/Desktop/CS2110/HW09/gba-makefiles/ship.png 1024@1024
 * /home/jshartar6/Desktop/CS2110/HW09/gba-makefiles/titleScreen.gif 320@200
 * 
 * All bug reports / feature requests are to be filed here https://github.com/TricksterGuy/nin10kit/issues
 */

#ifndef PICTURES_H
#define PICTURES_H

extern const unsigned short Kamikaze[130];
#define KAMIKAZE_SIZE 260
#define KAMIKAZE_LENGTH 130
#define KAMIKAZE_WIDTH 13
#define KAMIKAZE_HEIGHT 10

extern const unsigned short bullet[24];
#define BULLET_SIZE 48
#define BULLET_LENGTH 24
#define BULLET_WIDTH 3
#define BULLET_HEIGHT 8

extern const unsigned short enemy[1312];
#define ENEMY_SIZE 2624
#define ENEMY_LENGTH 1312
#define ENEMY_WIDTH 41
#define ENEMY_HEIGHT 32

extern const unsigned short enemyBullet[24];
#define ENEMYBULLET_SIZE 48
#define ENEMYBULLET_LENGTH 24
#define ENEMYBULLET_WIDTH 3
#define ENEMYBULLET_HEIGHT 8

extern const unsigned short gameover[72640];
#define GAMEOVER_SIZE 145280
#define GAMEOVER_LENGTH 72640
#define GAMEOVER_WIDTH 320
#define GAMEOVER_HEIGHT 227

extern const unsigned short ship[1048576];
#define SHIP_SIZE 2097152
#define SHIP_LENGTH 1048576
#define SHIP_WIDTH 1024
#define SHIP_HEIGHT 1024

extern const unsigned short titleScreen[64000];
#define TITLESCREEN_SIZE 128000
#define TITLESCREEN_LENGTH 64000
#define TITLESCREEN_WIDTH 320
#define TITLESCREEN_HEIGHT 200

#endif

