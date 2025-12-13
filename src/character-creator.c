#include "simple_logger.h"
#include "simple_json.h"

#include "character_creator.h"

CharacterCloset* closet = NULL;

//static SJson* appearanceDefs = NULL;

void character_creator_free()
{
	//
}

CharacterCloset* character_creator_get_closet()
{
	return closet;
}

void character_creator_closet_load()
{
	closet = gfc_allocate_array(sizeof(CharacterCloset), 1);
	if (!closet) {
		slog("failed to allocate appearance");
		return;
	}

	closet->body = gf3d_mesh_load("models/character/body.obj");

	closet->skincolors[0] = gf3d_texture_load("models/character/colors/skin4.png");
	closet->skincolors[1] = gf3d_texture_load("models/character/colors/skin3.png");
	closet->skincolors[2] = gf3d_texture_load("models/character/colors/skin2.png");
	closet->skincolors[3] = gf3d_texture_load("models/character/colors/skin5.png");
	closet->skincolors[4] = gf3d_texture_load("models/character/colors/skin7.png");
	closet->skincolors[5] = gf3d_texture_load("models/character/colors/skin6.png");

	closet->hair[0] = gf3d_mesh_load("models/character/hair/ponytail.obj");
	closet->hair[1] = gf3d_mesh_load("models/character/hair/bun.obj");
	closet->hair[2] = gf3d_mesh_load("models/character/hair/short.obj");
	closet->hair[3] = gf3d_mesh_load("models/character/hair/pigtails.obj");
	closet->hair[4] = gf3d_mesh_load("models/character/hair/bob.obj");

	closet->haircolors[0] = gf3d_texture_load("models/character/colors/black.png");
	closet->haircolors[1] = gf3d_texture_load("models/character/colors/skin7.png");
	closet->haircolors[2] = gf3d_texture_load("models/character/colors/skin1.png");
	closet->haircolors[3] = gf3d_texture_load("models/character/colors/blonde.png");
	closet->haircolors[4] = gf3d_texture_load("models/character/colors/purple.png");
	closet->haircolors[5] = gf3d_texture_load("models/character/colors/red.png");
	closet->haircolors[6] = gf3d_texture_load("models/character/colors/white.png");

	closet->tops[0] = gf3d_mesh_load("models/character/tops/tee.obj");
	closet->tops[1] = gf3d_mesh_load("models/character/tops/longsleeve.obj");
	closet->tops[2] = gf3d_mesh_load("models/character/tops/crewneck.obj");
	closet->tops[3] = gf3d_mesh_load("models/character/tops/shirt4.obj");

	closet->topcolors[0] = gf3d_texture_load("models/character/colors/red.png");
	closet->topcolors[1] = gf3d_texture_load("models/character/colors/black.png");
	closet->topcolors[2] = gf3d_texture_load("models/character/colors/purple.png");
	closet->topcolors[3] = gf3d_texture_load("models/character/colors/teal.png");
	closet->topcolors[4] = gf3d_texture_load("models/character/colors/green.png");
	closet->topcolors[5] = gf3d_texture_load("models/character/colors/pink.png");
	closet->topcolors[6] = gf3d_texture_load("models/character/colors/blue1.png");

	closet->bottoms[0] = gf3d_mesh_load("models/character/bottoms/jeans.obj");
	closet->bottoms[1] = gf3d_mesh_load("models/character/bottoms/skinny.obj");
	closet->bottoms[2] = gf3d_mesh_load("models/character/bottoms/flares.obj");
	closet->bottoms[3] = gf3d_mesh_load("models/character/bottoms/jorts.obj");
	closet->bottoms[4] = gf3d_mesh_load("models/character/bottoms/shorts.obj");
	closet->bottoms[5] = gf3d_mesh_load("models/character/bottoms/maxiskirt.obj");


	closet->bottomcolors[0] = gf3d_texture_load("models/character/colors/black.png");
	closet->bottomcolors[1] = gf3d_texture_load("models/character/colors/blue1.png");
	closet->bottomcolors[2] = gf3d_texture_load("models/character/colors/blue2.png");
	closet->bottomcolors[3] = gf3d_texture_load("models/character/colors/green.png");
	closet->bottomcolors[4] = gf3d_texture_load("models/character/colors/skin3.png");
	closet->bottomcolors[5] = gf3d_texture_load("models/character/colors/skin7.png");

	closet->shoes[0] = gf3d_mesh_load("models/character/shoes/sneakers.obj");
	closet->shoes[1] = gf3d_mesh_load("models/character/shoes/boots.obj");

	closet->shoecolors[0] = gf3d_texture_load("models/character/colors/skin6.png");
	closet->shoecolors[1] = gf3d_texture_load("models/character/colors/black.png");
	closet->shoecolors[2] = gf3d_texture_load("models/character/colors/green.png");
	closet->shoecolors[3] = gf3d_texture_load("models/character/colors/red.png");
}

CharacterAppearance* character_creator_change_appearance_all(CharacterAppearance* a, int skincolor, GFC_Vector4D clothing, GFC_Vector4D colors)
{
	if (!a) return;
	a->currentSkincolor = skincolor;
	a->currentHair = clothing.x;
	a->currentHaircolor = colors.x;
	a->currentTop = clothing.y;
	a->currentTopcolor = colors.y;
	a->currentBottom = clothing.z;
	a->currentBottomcolor = colors.z;
	a->currentShoes = clothing.w;
	a->currentShoecolor = colors.w;

	return a;
}

CharacterAppearance* character_creator_change_appearance(CharacterAppearance *a, const char* category)
{
	if (!a) return;
	if (strcmp(category, "skin") == 0)
	{
		a->currentSkincolor = (a->currentSkincolor + 1) % (sizeof(closet->skincolors) / sizeof(closet->skincolors[0]));
	}
	if (strcmp(category, "hair") == 0)
	{
		a->currentHair = (a->currentHair + 1) % (sizeof(closet->hair) / sizeof(closet->hair[0]));
	}
	if (strcmp(category, "haircolor") == 0)
	{
		a->currentHaircolor = (a->currentHaircolor + 1) % (sizeof(closet->haircolors) / sizeof(closet->haircolors[0]));
	}
	if (strcmp(category, "top") == 0)
	{
		a->currentTop = (a->currentTop + 1) % (sizeof(closet->tops) / sizeof(closet->tops[0]));
	}
	if (strcmp(category, "topcolor") == 0)
	{
		a->currentTopcolor = (a->currentTopcolor + 1) % (sizeof(closet->topcolors) / sizeof(closet->topcolors[0]));
	}
	if (strcmp(category, "bottom") == 0)
	{
		a->currentBottom = (a->currentBottom + 1) % (sizeof(closet->bottoms) / sizeof(closet->bottoms[0]));
	}
	if (strcmp(category, "bottomcolor") == 0)
	{
		a->currentBottomcolor = (a->currentBottomcolor + 1) % (sizeof(closet->bottomcolors) / sizeof(closet->bottomcolors[0]));
	}
	if (strcmp(category, "shoes") == 0)
	{
		a->currentShoes = (a->currentShoes + 1) % (sizeof(closet->shoes) / sizeof(closet->shoes[0]));
	}
	if (strcmp(category, "shoecolor") == 0)
	{
		a->currentShoecolor = (a->currentShoecolor + 1) % (sizeof(closet->shoecolors) / sizeof(closet->shoecolors[0]));
	}

	return a;
}

void character_creator_preview_draw(CharacterAppearance *a, GFC_Vector3D lightPos, GFC_Color lightColor)
{
	if ((!closet)||(!a)) return;
	//slog("drawing happening");
	GFC_Matrix4 modelMat;
	
	gfc_matrix4_from_vectors(
		modelMat,
		gfc_vector3d(10,0,-10),
		gfc_vector3d(0, 0, 3),
		gfc_vector3d(1.5, 1.5, 1.5));

	gf3d_mesh_draw(closet->body, modelMat, GFC_COLOR_WHITE, closet->skincolors[a->currentSkincolor], lightPos, lightColor);
	gf3d_mesh_draw(closet->hair[a->currentHair], modelMat, GFC_COLOR_WHITE, closet->haircolors[a->currentHaircolor], lightPos, lightColor);
	gf3d_mesh_draw(closet->tops[a->currentTop], modelMat, GFC_COLOR_WHITE, closet->topcolors[a->currentTopcolor], lightPos, lightColor);
	gf3d_mesh_draw(closet->bottoms[a->currentBottom], modelMat, GFC_COLOR_WHITE, closet->bottomcolors[a->currentBottomcolor], lightPos, lightColor);
	gf3d_mesh_draw(closet->shoes[a->currentShoes], modelMat, GFC_COLOR_WHITE, closet->shoecolors[a->currentShoecolor], lightPos, lightColor);
}

CharacterAppearance* character_creator_appearance_new()
{
	CharacterAppearance* a;
	a = gfc_allocate_array(sizeof(CharacterAppearance), 1);
	
	// default items
	a->currentSkincolor = 3;
	a->currentHair = 0;
	a->currentHaircolor = 1;
	a->currentTop = 0;
	a->currentTopcolor = 0;
	a->currentBottom = 0;
	a->currentBottomcolor = 1;
	a->currentShoes = 0;
	a->currentShoecolor = 0;

	return a;
}