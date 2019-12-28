#ifndef __MYLIB_H
#define __MYLIB_H

#include "lcd.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

//caro 3*3
void drawBoard(uint8_t a[][3]);
uint8_t checkWinner(uint8_t a[][3]);
void initBoard(uint8_t a[][3]);

#endif
