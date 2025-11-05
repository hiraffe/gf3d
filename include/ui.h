#ifndef __UI_H__
#define __UI_H__

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gfc_types.h"
#include "gfc_text.h"

typedef struct
{
    Sprite* background;
    int visible;
} UI_Manager;

/**
 * @brief get the ui manager
 * @return the ui manager
 */
UI_Manager ui_get_manager();

/**
 * @brief initializes the ui manager
 */
void ui_manager_init();

/**
 * @brief closes the ui manager
 */
void ui_manager_close();

/**
 * @brief draws the ui
 */
void ui_manager_draw();

/**
 * @brief makes the ui visible
 */
void ui_manager_show();

/**
 * @brief hides the ui
 */
void ui_manager_hide();

#endif