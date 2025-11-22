#ifndef __UI_H__
#define __UI_H__

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gfc_types.h"
#include "gfc_text.h"

#include "monster.h"

typedef struct UI_S
{
	Uint8				_inuse;
	GFC_TextLine		name;
	GFC_TextLine		title;
	GFC_TextLine		type;
	Sprite*				background;
	GFC_Vector2D		bg_position;
	Uint8				visible;
	Uint32				item_selected;
	Uint32				item_max;
	GFC_List*			buttons;

	void				(*draw)(struct UI_S* ui);
	void				(*think)(struct UI_S* ui);
	void				(*update)(struct UI_S* ui);
	void				(*free)(struct UI_S* ui);
	void*				data;
	MonsterEntityData*	mData;
}UI;

typedef struct
{
    UI* ui_list;
    Uint32 ui_max;
}UI_Manager;

/**
 * @brief get the ui manager
 * @return the ui manager
 */
UI_Manager ui_get_manager();

/**
 * @brief get a pointer to a new blank entity
 * @return NULL on out of memory or other error, pointer to blank entity otherwise
 */
UI* ui_new();

void ui_free(UI* ui);

/**
 * @brief initializes the ui manager
 */
void ui_manager_init(Uint32);

/**
 * @brief closes the ui manager
 */
void ui_manager_close();

/**
 * @brief draws the ui
 */
void ui_manager_draw_all();

void ui_manager_think_all();

#endif