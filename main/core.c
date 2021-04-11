#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "sdkconfig.h"
#include "audio_element.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "input_key_service.h"

#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "board.h"

#include "audio_mem.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"

#include "i2c-lcd1602.h"
#include "smbus.h"

#include "core.h"
#include "spaceInvader.h"
#include "menu.h"
#include "lcd.h"

//Log variable
static const char *TAG = "MENU";

//Standard key method annotations
int keyScanWin(void);
int keyScan(void);
int keyScanNonBlocking(void);

//LCD variables and method annotations
smbus_info_t *smbus_info = NULL;
i2c_lcd1602_info_t *lcd_info = NULL;

void loadMainMenuLCDDisplay(MENU_ITEM_T);

//Button handlers
void setButtonHandler();
void playButtonHandler();
void modeButtonHandler();
void recButtonHandler();

//Timer variables and method annotations
esp_timer_handle_t space_timer;

void spaceTimer();
static void periodicTimerCallback(void *arg);
void stopSpaceTimer();

//Space invader variables and method annotations
GAME_T reset_game = {
	{{3, {LCD_Row_1, LCD_Max}, true}, {7, {LCD_Row_3, LCD_Max}, true}, {9, {LCD_Row_4, LCD_Max}, true}, {12, {LCD_Row_3, LCD_Max}, true}, {16, {LCD_Row_1, LCD_Max}, true}, {18, {LCD_Row_2, LCD_Max}, true}, {20, {LCD_Row_4, LCD_Max}, true}, {22, {LCD_Row_1, LCD_Max}, true}, {24, {LCD_Row_1, LCD_Max}, true}, {27, {LCD_Row_2, LCD_Max}, true}, {30, {LCD_Row_3, LCD_Max}, true}, {33, {LCD_Row_3, LCD_Max}, true}, {33, {LCD_Row_2, LCD_Max}, true}, {36, {LCD_Row_4, LCD_Max}, true}, {40, {LCD_Row_4, LCD_Max}, true}, {41, {LCD_Row_1, LCD_Max}, true}, {42, {LCD_Row_2, LCD_Max}, true}, {44, {LCD_Row_4, LCD_Max}, true}, {46, {LCD_Row_3, LCD_Max}, true}, {48, {LCD_Row_2, LCD_Max}, true}, {50, {LCD_Row_2, LCD_Max}, true}, {53, {LCD_Row_1, LCD_Max}, true}, {55, {LCD_Row_1, LCD_Max}, true}, {58, {LCD_Row_3, LCD_Max}, true}, {61, {LCD_Row_4, LCD_Max}, true}, {64, {LCD_Row_3, LCD_Max}, true}, {64, {LCD_Row_4, LCD_Max}, true}, {68, {LCD_Row_3, LCD_Max}, true}, {68, {LCD_Row_2, LCD_Max}, true}, {72, {LCD_Row_2, LCD_Max}, true}, {72, {LCD_Row_1, LCD_Max}, true}, {78, {LCD_Row_2, LCD_Max}, true}, {78, {LCD_Row_3, LCD_Max}, true}, {78, {LCD_Row_4, LCD_Max}, true}, {82, {LCD_Row_2, LCD_Max}, true}, {82, {LCD_Row_1, LCD_Max}, true}, {82, {LCD_Row_4, LCD_Max}, true}, {85, {LCD_Row_2, LCD_Max}, true}, {85, {LCD_Row_3, LCD_Max}, true}, {90, {LCD_Row_3, LCD_Max}, true}, {90, {LCD_Row_2, LCD_Max}, true}, {92, {LCD_Row_2, LCD_Max}, true}, {92, {LCD_Row_1, LCD_Max}, true}, {94, {LCD_Row_1, LCD_Max}, true}, {94, {LCD_Row_4, LCD_Max}, true}, {96, {LCD_Row_4, LCD_Max}, true}, {96, {LCD_Row_3, LCD_Max}, true}, {96, {LCD_Row_2, LCD_Max}, true}, {99, {LCD_Row_4, LCD_Max}, true}, {100, {LCD_Row_3, LCD_Max}, true}},
	{LCD_Row_3, LCD_Min},
	0,
	{{false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {0, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}}};

GAME_T game = {
	{{3, {LCD_Row_1, LCD_Max}, true}, {7, {LCD_Row_3, LCD_Max}, true}, {9, {LCD_Row_4, LCD_Max}, true}, {12, {LCD_Row_3, LCD_Max}, true}, {16, {LCD_Row_1, LCD_Max}, true}, {18, {LCD_Row_2, LCD_Max}, true}, {20, {LCD_Row_4, LCD_Max}, true}, {22, {LCD_Row_1, LCD_Max}, true}, {24, {LCD_Row_1, LCD_Max}, true}, {27, {LCD_Row_2, LCD_Max}, true}, {30, {LCD_Row_3, LCD_Max}, true}, {33, {LCD_Row_3, LCD_Max}, true}, {33, {LCD_Row_2, LCD_Max}, true}, {36, {LCD_Row_4, LCD_Max}, true}, {40, {LCD_Row_4, LCD_Max}, true}, {41, {LCD_Row_1, LCD_Max}, true}, {42, {LCD_Row_2, LCD_Max}, true}, {44, {LCD_Row_4, LCD_Max}, true}, {46, {LCD_Row_3, LCD_Max}, true}, {48, {LCD_Row_2, LCD_Max}, true}, {50, {LCD_Row_2, LCD_Max}, true}, {53, {LCD_Row_1, LCD_Max}, true}, {55, {LCD_Row_1, LCD_Max}, true}, {58, {LCD_Row_3, LCD_Max}, true}, {61, {LCD_Row_4, LCD_Max}, true}, {64, {LCD_Row_3, LCD_Max}, true}, {64, {LCD_Row_4, LCD_Max}, true}, {68, {LCD_Row_3, LCD_Max}, true}, {68, {LCD_Row_2, LCD_Max}, true}, {72, {LCD_Row_2, LCD_Max}, true}, {72, {LCD_Row_1, LCD_Max}, true}, {78, {LCD_Row_2, LCD_Max}, true}, {78, {LCD_Row_3, LCD_Max}, true}, {78, {LCD_Row_4, LCD_Max}, true}, {82, {LCD_Row_2, LCD_Max}, true}, {82, {LCD_Row_1, LCD_Max}, true}, {82, {LCD_Row_4, LCD_Max}, true}, {85, {LCD_Row_2, LCD_Max}, true}, {85, {LCD_Row_3, LCD_Max}, true}, {90, {LCD_Row_3, LCD_Max}, true}, {90, {LCD_Row_2, LCD_Max}, true}, {92, {LCD_Row_2, LCD_Max}, true}, {92, {LCD_Row_1, LCD_Max}, true}, {94, {LCD_Row_1, LCD_Max}, true}, {94, {LCD_Row_4, LCD_Max}, true}, {96, {LCD_Row_4, LCD_Max}, true}, {96, {LCD_Row_3, LCD_Max}, true}, {96, {LCD_Row_2, LCD_Max}, true}, {99, {LCD_Row_4, LCD_Max}, true}, {100, {LCD_Row_3, LCD_Max}, true}},
	{LCD_Row_3, LCD_Min},
	0,
	{{false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {0, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}, {false, {LCD_Row_1, LCD_Max}}}};

int lastPressed = Empty;
int gameState = MenuState;
int winLossTimer = 0;
int highScore = 0;
int currentScore = 0;
int gameRunning = false;

void initInvaders(void);
void inputCheck(int);
void moveUp();
void moveDown();
void shoot();
void moveShots();
void checkEnemies();
void moveEnemies();
void checkGameState();
int checkScoreHighScore();

//Board variables
audio_board_handle_t board_handleTalkingClock;
audio_event_iface_handle_t evt, evtTalkingClock;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//LCD

//This method sets the LCD to the main menu screen.
void loadMainMenuLCDDisplay(MENU_ITEM_T menuItem)
{
	i2c_lcd1602_clear(lcd_info);

	//Sets space invaders side decoration
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_1);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_2);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_3);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_4);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_1);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_2);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_3);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_4);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);

	//sets the correct text thanks to the menu item given as the parameter
	i2c_lcd1602_move_cursor(lcd_info, 3, LCD_Row_1);
	i2c_lcd1602_write_string(lcd_info, menuItem.row1);
	i2c_lcd1602_move_cursor(lcd_info, 5, LCD_Row_2);
	i2c_lcd1602_write_string(lcd_info, menuItem.row2);
	i2c_lcd1602_move_cursor(lcd_info, 5, LCD_Row_3);
	i2c_lcd1602_write_string(lcd_info, menuItem.row3);

	//converts the integer of highscore to a string so it can be shown
	char result[50];
	sprintf(result, "%d", highScore);
	i2c_lcd1602_move_cursor(lcd_info, 8, LCD_Row_4);
	i2c_lcd1602_write_string(lcd_info, result);
}

//This method sets the LCD to the lose screen.
void loadLoseScreenLCDDisplay(MENU_ITEM_T menuItem)
{
	i2c_lcd1602_clear(lcd_info);

	//Sets space invaders side decoration
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_1);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_2);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_3);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_4);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_1);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_2);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_3);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_4);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2);

	//sets the correct text thanks to the menu item given as the parameter
	char result[50];
	i2c_lcd1602_move_cursor(lcd_info, 2, LCD_Row_1);
	i2c_lcd1602_write_string(lcd_info, menuItem.row1);
	i2c_lcd1602_move_cursor(lcd_info, 5, LCD_Row_2);
	i2c_lcd1602_write_string(lcd_info, menuItem.row4);

	//converts the integer of highscore to a string so it can be shown
	if (checkScoreHighScore() == true)
	{
		i2c_lcd1602_move_cursor(lcd_info, 3, LCD_Row_3);
		i2c_lcd1602_write_string(lcd_info, menuItem.row2);
		sprintf(result, "%d", highScore);
	}
	else
	{
		i2c_lcd1602_move_cursor(lcd_info, 5, LCD_Row_3);
		i2c_lcd1602_write_string(lcd_info, menuItem.row3);
		sprintf(result, "%d", currentScore);
	}

	i2c_lcd1602_move_cursor(lcd_info, 8, LCD_Row_4);
	i2c_lcd1602_write_string(lcd_info, result);
}

//This method sets the LCD to the win screen.
void loadWinScreenLCDDisplay(MENU_ITEM_T menuItem)
{
	i2c_lcd1602_clear(lcd_info);

	//Sets space invaders side decoration
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_1);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_2);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_3);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_Min, LCD_Row_4);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_1);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_2);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_3);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);
	i2c_lcd1602_move_cursor(lcd_info, LCD_End, LCD_Row_4);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);

	//sets the correct text thanks to the menu item given as the parameter
	char result[50];
	i2c_lcd1602_move_cursor(lcd_info, 2, LCD_Row_1);
	i2c_lcd1602_write_string(lcd_info, menuItem.row1);
	i2c_lcd1602_move_cursor(lcd_info, 5, LCD_Row_2);
	i2c_lcd1602_write_string(lcd_info, menuItem.row2);

	//converts the integer of highscore to a string so it can be shown
	if (checkScoreHighScore() == true)
	{
		i2c_lcd1602_move_cursor(lcd_info, 3, LCD_Row_3);
		i2c_lcd1602_write_string(lcd_info, menuItem.row3);
		sprintf(result, "%d", highScore);
	}
	else
	{
		i2c_lcd1602_move_cursor(lcd_info, 5, LCD_Row_3);
		i2c_lcd1602_write_string(lcd_info, menuItem.row4);
		sprintf(result, "%d", currentScore);
	}

	i2c_lcd1602_move_cursor(lcd_info, 8, LCD_Row_4);
	i2c_lcd1602_write_string(lcd_info, result);
}

//This method allows the actual game to show on the LCD
void updateSpaceOnLCD()
{
	i2c_lcd1602_clear(lcd_info);

	//Player gets printed
	i2c_lcd1602_move_cursor(lcd_info, game.Player.xPos, game.Player.yPos);
	i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1);

	//enemies that are relevant get printed
	for (int i = 0; i < EnemiesLimit; i++)
	{
		if (game.timeCounter == game.enemies[i].entryTurn && game.enemies[i].isAlive == true)
		{
			//The alive enemies
			i2c_lcd1602_move_cursor(lcd_info, game.enemies[i].position.xPos, game.enemies[i].position.yPos);
			i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_2);
			game.enemies[i].entryTurn++;
		}
		else if (game.timeCounter == game.enemies[i].entryTurn && game.enemies[i].isAlive == false)
		{
			//The freshly killed enemies
			i2c_lcd1602_move_cursor(lcd_info, game.enemies[i].position.xPos, game.enemies[i].position.yPos);
			i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_4);
		}
	}

	//shots in travel are printed
	for (int i = 0; i < ShotsLimit; i++)
	{
		if (game.shots[i].isValid == true)
		{
			i2c_lcd1602_move_cursor(lcd_info, game.shots[i].position.xPos, game.shots[i].position.yPos);
			i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_3);
		}
	}
}

//This method initializes the LCD and sets the custom characters for space invaders.
void init_lcd(void)
{
	// Set up the SMBus
	smbus_info = smbus_malloc();
	smbus_init(smbus_info, I2C_NUM_0, LCD_ADDRESS);
	smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

	// Set up the LCD1602 device with backlight off
	lcd_info = i2c_lcd1602_malloc();
	i2c_lcd1602_init(lcd_info, smbus_info, true, LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VIS_COLUMNS);

	//set up for space invader custom characters
	uint8_t player[8] = {0x4, 0x6, 0xf, 0x1e, 0xf, 0x6, 0x4};
	i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_1, player);

	uint8_t enemy[8] = {0x18, 0xc, 0x1e, 0xf, 0x1e, 0xc, 0x18};
	i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_2, enemy);

	uint8_t shot[8] = {0x0, 0x0, 0x0, 0xe, 0x0, 0x0, 0x0};
	i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_3, shot);

	uint8_t kill[8] = {0x0, 0x15, 0xe, 0x1f, 0xe, 0x15, 0x0};
	i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_4, kill);

	// Write first line
	i2c_lcd1602_clear(lcd_info);
}

//-------------------------------------------------------------------------------------------------
//Button handlers

//This handler sets the last pressed button to up
void modeButtonHandler()
{
	if (lastPressed == Empty)
	{
		lastPressed = Up;
	}
}

//This handler sets the last pressed button to down
void recButtonHandler()
{
	if (lastPressed == Empty)
	{
		lastPressed = Down;
	}
}

//This handler sets the last pressed button to shoot
void setButtonHandler()
{
	if (lastPressed == Empty)
	{
		lastPressed = Shoot;
	}
}

//This handler sets the last pressed button to play
void playButtonHandler()
{
	if (lastPressed == Empty)
	{
		lastPressed = Play;
	}
}

//This is the input key service handler for the board, specificly for mode, rec, play and set as these are needed for the controls
static esp_err_t input_key_service_cb(periph_service_handle_t handle, periph_service_event_t *evt, void *ctx)
{
	/* Handle touch pad events
           to start, pause, resume, finish current song and adjust volume
        */
	//Stucture from essprissif, editted to fit its needs
	audio_board_handle_t board_handle = (audio_board_handle_t)ctx;
	int player_volume;
	audio_hal_get_volume(board_handle->audio_hal, &player_volume);

	if (evt->type == INPUT_KEY_SERVICE_ACTION_CLICK_RELEASE)
	{
		switch ((int)evt->data)
		{
		case INPUT_KEY_USER_ID_PLAY:
		{
			playButtonHandler();
			break;
		}
		case INPUT_KEY_USER_ID_SET:
		{
			setButtonHandler();
			break;
		}
		case INPUT_KEY_USER_ID_MODE:
		{
			modeButtonHandler();
			break;
		}
		case INPUT_KEY_USER_ID_REC:
		{
			recButtonHandler();
			break;
		}
		}
	}

	return ESP_OK;
}

//-------------------------------------------------------------------------------------------------
//Main

// Main entry point and initializer of the board and timer
int coreStart()
{
	//Code from Esppressif from here
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}

	esp_netif_init();

	nvs_handle_t my_handle;
	err = nvs_open("storage", NVS_READWRITE, &my_handle);
	int32_t current = 0; // value will default to 0, if not set yet in NVS
	err = nvs_get_i32(my_handle, "current", &current);

	// Setup logging level
	esp_log_level_set("*", ESP_LOG_INFO);
	esp_log_level_set(TAG, ESP_LOG_INFO);

	ESP_LOGI(TAG, "[1.0] Initialize peripherals management");
	esp_periph_config_t periph_cfgTalkingClock = DEFAULT_ESP_PERIPH_SET_CONFIG();
	esp_periph_set_handle_t setTalkingClock = esp_periph_set_init(&periph_cfgTalkingClock);

	ESP_LOGI(TAG, "[1.1] Initialize and start peripherals");
	audio_board_key_init(setTalkingClock);

	ESP_LOGI(TAG, "[ 2 ] Start codec chip");
	board_handleTalkingClock = audio_board_init();
	audio_hal_ctrl_codec(board_handleTalkingClock->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

	ESP_LOGI(TAG, "[ 3 ] Create and start input key service");
	input_key_service_info_t input_key_info[] = INPUT_KEY_DEFAULT_INFO();
	input_key_service_cfg_t input_cfg = INPUT_KEY_SERVICE_DEFAULT_CONFIG();
	input_cfg.handle = setTalkingClock;
	periph_service_handle_t input_ser = input_key_service_create(&input_cfg);
	input_key_service_add_key(input_ser, input_key_info, INPUT_KEY_NUM);
	periph_service_set_callback(input_ser, input_key_service_cb, (void *)board_handleTalkingClock);
	ESP_LOGI(TAG, "[ 4 ] init LCD");
	//Code from Esppressif until here

	init_lcd();
	loadMainMenuLCDDisplay(retrieveMenu());
	spaceTimer();
	while (true)
	{
		vTaskDelay(10);
	}
	printf("Goodbye\n");
	return 1;
}

//-------------------------------------------------------------------------------------------------
//Timer

//This method initializes the timer and start it.
//Not my own code, from online
void spaceTimer()
{
	const esp_timer_create_args_t snake_timer_args = {
		.callback = &periodicTimerCallback,
		/* name is optional, but may help identify the timer when debugging */
		.name = "periodic"};

	ESP_ERROR_CHECK(esp_timer_create(&snake_timer_args, &space_timer));

	/* Start the timers */
	ESP_ERROR_CHECK(esp_timer_start_periodic(space_timer, TurnTime));
}

//This is the callback for the timer, this plays the chosen move and starts the correct actions
static void periodicTimerCallback(void *arg)
{
	//Here we check whether the gamestate is for the menu if so until the player taps play the game won't begin
	if (gameState == MenuState)
	{
		if (lastPressed == Play)
		{
			initInvaders();
		}
	}
	//Here the game is played and all commands are handled to play the game
	if (gameState == InGameState)
	{
		inputCheck(lastPressed);
		if (gameRunning == true)
		{
			updateSpaceOnLCD();
		}
		lastPressed = Empty;
	}
	//Here the player gets shown the win/lose screen until the time runs out and returns to the main menu
	if (gameState == WinLossState)
	{
		winLossTimer++;
		if (winLossTimer >= WinLossTime)
		{
			loadMainMenuLCDDisplay(retrieveMenu());
			gameState = MenuState;
			winLossTimer = 0;
		}
	}
}

//This method stops the timer
//		(not used, prepared for exspansion)
//Not my own code, from online
void stopSpaceTimer()
{
	esp_timer_handle_t periodic_timer_handle = (esp_timer_handle_t)spaceTimer;
	ESP_ERROR_CHECK(esp_timer_stop(periodic_timer_handle));
}

//-------------------------------------------------------------------------------------------------
//Game methods

//This method resets the game to its original state so the player can play again
void initInvaders()
{
	game = reset_game;
	gameState = InGameState;
	currentScore = 0;
	gameRunning = true;
}

//This method checks whether an input is correct, if so handle the command and do it else ignore and continue the game.
void inputCheck(int action)
{
	game.timeCounter++;
	switch (action)
	{
	case Up:
		moveUp();
		break;
	case Down:
		moveDown();
		break;
	case Shoot:
		shoot();
		break;
	default:
		moveShots();
		break;
	}
}

//This moves the player 1 up if possible else it will stay at the top and continue the game
void moveUp()
{
	if (game.Player.yPos > LCD_Min)
	{
		game.Player.yPos--;
	}
	moveShots();
}

//This moves the player 1 down if possible else it will stay at the bottom and continue the game
void moveDown()
{
	if (game.Player.yPos < LCD_Bottom)
	{
		game.Player.yPos++;
	}
	moveShots();
}

//This allows the player to shoot one shot and continue the game (it also adds the new shot to the list to track)
void shoot()
{
	moveShots();
	for (int i = 0; i < ShotsLimit; i++)
	{
		if (game.shots[i].isValid == false)
		{
			game.shots[i].isValid = true;
			game.shots[i].position.xPos = 1;
			game.shots[i].position.yPos = game.Player.yPos;
			break;
		}
	}
	currentScore -= ShotCost;
}

//This method moves all shots by 1 to the right and continues the process of the turn
void moveShots()
{
	for (int i = 0; i < ShotsLimit; i++)
	{
		if (game.shots[i].isValid == true)
		{
			game.shots[i].position.xPos++;
			if (game.shots[i].position.xPos == LCD_Max)
			{
				game.shots[i].isValid = false;
			}
		}
	}
	checkEnemies();
	moveEnemies();
	checkGameState();
}

//This method checks whether an enemy died from a shot or not.
void checkEnemies()
{
	for (int i = 0; i < EnemiesLimit; i++)
	{
		if (game.shots[i].isValid == true)
		{

			for (int j = 0; j < EnemiesLimit; j++)
			{
				if (game.enemies[j].isAlive)
				{
					if (game.enemies[j].position.xPos == game.shots[i].position.xPos && game.enemies[j].position.yPos == game.shots[i].position.yPos)
					{
						game.shots[i].isValid = false;
						game.enemies[j].isAlive = false;
						currentScore += EnemyKilledReward;
					}
				}
			}
		}
	}
}

//This method moves all relevant enemies to the left by one, this method also check whether the game is lost or not
void moveEnemies()
{
	for (int i = 0; i < EnemiesLimit; i++)
	{
		if (game.enemies[i].isAlive == true && game.enemies[i].entryTurn == game.timeCounter)
		{
			game.enemies[i].position.xPos--;
			if (game.enemies[i].position.xPos == LCD_Min)
			{
				gameState = WinLossState;
				loadLoseScreenLCDDisplay(retrieveLose());
				gameRunning = false;
			}
		}
	}
	checkEnemies();
}

//This method checks whether the gamestate needs to change to a win (all enemies are dead)
void checkGameState()
{
	int result = 0;
	for (int i = 0; i < EnemiesLimit; i++)
	{
		if (game.enemies[i].isAlive == true)
		{
			result++;
		}
	}
	if (result == 0)
	{
		gameState = WinLossState;
		loadWinScreenLCDDisplay(retrieveWin());
		gameRunning = false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Score

//this method checks whether the current score reached is higher than the highscore.
int checkScoreHighScore()
{
	if (currentScore > highScore)
	{
		highScore = currentScore;
		return true;
	}
	return false;
}
