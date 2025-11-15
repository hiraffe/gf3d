#ifndef __SHOP_MENU__
#define __SHOP_MENU__

#include "shop.h"

#include "ui.h"

typedef enum
{
	SS_Buy,
	SS_Sell
}ShopState;

typedef struct
{
	ShopState		state;
	ShopEntityData* shopData;
}ShopMenuData;

/**
 * @brief spawn in a new shop menu
 * @return NULL on error, or a new shop menu
 */
UI* shop_menu_new(ShopEntityData* shop, const char* name);

/**
 * @brief make the shop menu visible
 */
void shop_menu_open(UI* ui);

/**
 * @brief hide the shop menu
 */
void shop_menu_close(UI* ui);


#endif