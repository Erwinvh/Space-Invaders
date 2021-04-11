#ifndef MENUSTRUCT_H
#define MENUSTRUCT_H

#include "lcd.h"

//The menu Item Struct is a struct that is used for all menu screens, not the in game screen.
typedef struct
{
	char row1[LCD_Max];
	char row2[LCD_Max];
	char row3[LCD_Max];
	char row4[LCD_Max];
} MENU_ITEM_T;

//The getter of the main menu struct
MENU_ITEM_T retrieveMenu(void);

//The getter of the lose screen struct
MENU_ITEM_T retrieveLose(void);

//The getter of the win screen struct
MENU_ITEM_T retrieveWin(void);

#endif
