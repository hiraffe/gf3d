#ifndef __CHARACTER_CREATOR_H_
#define __CHARACTER_CREATOR_H_

#include "entity.h"

typedef struct
{
	Mesh* body;
	Texture* skincolors[8];
	Mesh* hair[8];
	Texture* haircolors[8];
	Mesh* tops[4];
	Texture* topcolors[8];
	Texture* bottomcolors[8];
	Mesh* shoes[2];
	Texture* shoecolors[8];

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

void character_preview_draw(CharacterAppearance* a, GFC_Vector3D lightPos, GFC_Color lightColor);

CharacterAppearance* character_appearance_load();

#endif