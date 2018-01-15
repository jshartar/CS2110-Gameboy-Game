#include "myLib.h"

u16* videoBuffer = (u16*)0x6000000;

void setPixel(int row, int col, u16 color) {
    videoBuffer[row * 240 + col] = color;
}

void waitForV() {
    while(SCANLINECOUNTER > 160);
    while(SCANLINECOUNTER < 160);
}

void drawImage3(int row, int col, int width, int height, const u16* image) {
    for (int i = 0; i < height; i++) {
        DMA[3].src = image + (i * (width));
        DMA[3].dst = videoBuffer + ((i + col) * 240) + (row);
        DMA[3].cnt = (width) | DMA_ON | DMA_SOURCE_INCREMENT;
    }
}

void drawRectangle(int x, int y, int width, int height, u16 color) {
    for (int i = 0; i < height; i++ ) {
        DMA[3].src = &color;
        DMA[3].dst = videoBuffer + ((i + y) * 240) + (x);
        DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
    }
}

void drawBlankRectangle(int x, int y, int width, int height, u16 color) {
    for (int i = 0; i < height; i++) {
        setPixel(x, (i + y), color);
        setPixel((x + width), (i + y), color);
    }
    for (int j = 0; j < width; j++) {
        setPixel((x + j), y, color);
        setPixel((x + j), (y + height), color);
    }
}
