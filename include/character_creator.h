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

	int currentSkincolor;
	int currentHair;
	int currentHaircolor;
	int currentTop;
	int currentTopcolor;
	int currentBottomcolor;
	int currentShoes;
	int currentShoecolor;
}CharacterAppearance;

typedef struct {
	CharacterAppearance appearance;
} CharacterPreview;

void character_preview_draw(GFC_Vector3D lightPos, GFC_Color lightColor);

void character_change_appearance(const char* category);

CharacterAppearance* character_appearance_load();

CharacterAppearance* character_update_appearance();

#endif