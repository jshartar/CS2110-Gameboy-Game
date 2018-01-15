cmake_minimum_required(VERSION 3.8)
project(gba_makefiles)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        res/arm-gba.ld
        res/crt0.s
        res/GBAVariables.mak
        res/libc_sbrk.c
        Makefile myLib.h main.c bullet.h enemy.h enemyBullet.h gameOver.h kamikaze.h startScreen.h ship.h)

add_executable(gba_makefiles ${SOURCE_FILES})