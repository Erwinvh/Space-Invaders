#include "menu.h"

//The values of the main menu struct, this allows the main menu to contain all the correct words.
MENU_ITEM_T menu = {
	"Space Invaders",
	"press play",
	"Highscore: ",
	""};

//The values of the lose screen struct, this allows the lose screen to contain all the correct words.
MENU_ITEM_T loseScreen = {
	"Sorry, you lost",
	"New Highscore!",
	"Your score: ",
	"Try again!"};

//The values of the win screen struct, this allows the win screen to contain all the correct words.
MENU_ITEM_T winScreen = {
	"Congratulations!",
	"You Won!",
	"Your score: ",
	"New Highscore!"};

//The getter of the main menu struct
MENU_ITEM_T retrieveMenu()
{
	return menu;
}

//The getter of the lose screen struct
MENU_ITEM_T retrieveLose()
{
	return loseScreen;
}

//The getter of the win screen struct
MENU_ITEM_T retrieveWin()
{
	return winScreen;
}