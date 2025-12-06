#ifndef __CHARACTER_CREATOR_H_
#define __CHARACTER_CREATOR_H_

#include "entity.h"

typedef struct
{
	Mesh* body;
	Texture* skincolors[1];
	Mesh* hair[2];
	Texture* haircolors[1];
	Mesh* tops[2];
	Texture* topcolors[1];
	Texture* bottomcolors[8];
	Mesh* shoes[1];
	Texture* shoecolors[1];
}CharacterCloset;

typedef struct {
	int currentSkincolor;
	int currentHair;
	int currentHaircolor;
	int currentTop;
	int currentTopcolor;
	int currentBottom;
	int currentBottomcolor;
	int currentShoes;
	int currentShoecolor;
}CharacterAppearance;


void character_creator_preview_draw(CharacterAppearance* a, GFC_Vector3D lightPos, GFC_Color lightColor);

CharacterAppearance* character_creator_change_appearance_all(CharacterAppearance* a, int skincolor, GFC_Vector4D clothing, GFC_Vector4D colors);

CharacterAppearance* character_creator_change_appearance(CharacterAppearance* a, const char* category);

void character_creator_closet_load();

CharacterCloset* character_creator_get_closet();

CharacterAppearance* character_creator_appearance_new();

#endif