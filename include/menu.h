#ifndef __MENU_H__
#define __MENU_H__

#include "ui.h"

typedef enum
{
	GS_MainMenu,
	GS_CharacterCreator,
	GS_Play,
	GS_Pause,
	GS_Quit
}GameState;

typedef struct
{
	GFC_TextLine name;
	GFC_TextLine text;
	GFC_TextLine command;
	GFC_Vector2D position;
}Menu_Button;

/**
 * @brief spawn in a new start menu
 * @return NULL on error, or a new start menu
 */
UI* menu_new(const char* name);

/**
 * @brief make the start menu visible
 */
void menu_open(UI* ui);

/**
 * @brief hide the start menu
 */
void menu_close(UI* ui);

GameState menu_get_game_state();

void menu_set_game_state(GameState g);

void menu_play_selected();

void menu_play_click();

void menu_init(const char* filename);


#endif