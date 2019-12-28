#include "mylib.h"

void drawBoard(uint8_t a[][3])
{
   char *_title = "Tic Tac Toe";
   char _line0[10] = "-------";
   char _line1[10] = "| | | |";
   char _line2[10] = "-------";
   char _line3[10] = "| | | |";
   char _line4[10] = "-------";
   char _line5[10] = "| | | |";
   char _line6[10] = "-------";
   char _winner_str[6] = "_ win";
   for (int8_t i = 0; i < 3; i++)
   {
      _line1[2 * i + 1] = a[0][i] == 1 ? 'X' : (a[0][i] == 2 ? 'O' : ' ');
      _line3[2 * i + 1] = a[1][i] == 1 ? 'X' : (a[1][i] == 2 ? 'O' : ' ');
      _line5[2 * i + 1] = a[2][i] == 1 ? 'X' : (a[2][i] == 2 ? 'O' : ' ');
   }
   LCD_DisplayStringLine(LCD_LINE_0, (uint8_t *)_title);
   LCD_DisplayStringLine(LCD_LINE_1, (uint8_t *)_line0);
   LCD_DisplayStringLine(LCD_LINE_2, (uint8_t *)_line1);
   LCD_DisplayStringLine(LCD_LINE_3, (uint8_t *)_line2);
   LCD_DisplayStringLine(LCD_LINE_4, (uint8_t *)_line3);
   LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)_line4);
   LCD_DisplayStringLine(LCD_LINE_6, (uint8_t *)_line5);
   LCD_DisplayStringLine(LCD_LINE_7, (uint8_t *)_line6);
   uint8_t _the_winner = checkWinner(a);
   if (_the_winner)
   {
      _winner_str[0] = _the_winner == 1 ? 'X' : 'O';
      LCD_DisplayStringLine(LCD_LINE_8, (uint8_t *)_winner_str);
      //LCD_DisplayStringLine(LCD_LINE_8, _the_winner == 1 ? 'X' : 'O');
   }
}
uint8_t checkWinner(uint8_t a[][3])
{
   for (uint8_t i = 0; i < 3; i++)
   {
      if (a[0][i] == a[1][i] && a[1][i] == a[2][i] && a[1][i] != 0)
      {
         return a[0][i];
      }
      if (a[i][0] == a[i][1] && a[i][1] == a[i][2] && a[i][1] != 0)
      {
         return a[i][0];
      }
   }
   if (a[1][1] == 0)
      return 0;
   if (a[0][0] == a[1][1] && a[1][1] == a[2][2])
   {
      return a[0][0];
   }
   if (a[2][0] == a[1][1] && a[1][1] == a[0][2])
   {
      return a[2][0];
   }
   return 0;
}
void initBoard(uint8_t a[][3])
{
   for (uint8_t i = 0; i < 3; i++)
   {
      for (uint8_t j = 0; j < 3; j++)
      {
         a[i][j] = 0;
      }
   }
}
