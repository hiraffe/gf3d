#ifndef __NPC_H_
#define __NPC_H_

#include "character_creator.h"
#include "ui.h"

#include "entity.h"

typedef struct
{
	Uint32					skin;
	GFC_Vector4D			clothing;
	GFC_Vector4D			colors;
	GFC_TextLine			dialogue1;
	GFC_TextLine			dialogue2;
	UI*						dialogue_box;
	CharacterAppearance*	appearance;
	Uint8					spokenTo;
}NPCEntityData;

void npc_init(const char* filename);

/**
 * @brief spawn in a new npc
 * @return NULL on error, or a new npc
 */
Entity* npc_spawn(const char* name);

void npc_draw(Entity* self, GFC_Vector3D lightPos, GFC_Color lightColor);


#endif