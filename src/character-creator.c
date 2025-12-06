#include "simple_logger.h"
#include "simple_json.h"

#include "character_creator.h"

CharacterCloset* closet = NULL;

//static SJson* appearanceDefs = NULL;

void character_creator_free()
{
	//
}

/*
void character_creator_init(const char* filename)
{
	SJson* json;
	json = sj_load(filename);
	appearanceDefs = sj_object_get_value(json, "appearance");
	if (!appearanceDefs)
	{
		slog("appearance definition file %s does not contain list", filename);
		sj_free(json);
		json = NULL;
		return;
	}

	atexit(character_creator_free);
}

SJson* character_creator_get_category_by_name(const char* name)
{
	int i, c;
	SJson* category;
	const char* categoryName = NULL;
	if (!name) return NULL;
	if (!appearanceDefs)
	{
		slog("no appearance definitions loaded");
		return NULL;
	}
	c = sj_array_get_count(appearanceDefs);
	for (int i = 0; i < c; i++)
	{
		category = sj_array_get_nth(appearanceDefs, i);
		if (!category) continue;
		categoryName = sj_object_get_value_as_string(category, "name");
		if (!categoryName) continue;
		if (gfc_strlcmp(name, categoryName) == 0) {
			//found it
			return category;
		}
	}
	slog("no category found by name", name);
	return NULL;
}

SJson* character_creator_get_categories(CharacterAppearance *a, SJson* json)
{
	SJson* options = sj_object_get_value(json, "options");
	SJson* colors = sj_object_get_value(json, "colors");
	int optCount = sj_array_get_count(options);
	int colorCount = sj_array_get_count(colors);
	GFC_Vector2D pos = { 0 };

	/*
	for (int i = 0; i < buttonCount; i++)
	{
		SJson* btnJson = sj_array_get_nth(buttons, i);
		Menu_Button* btn = gfc_allocate_array(sizeof(Menu_Button), 1);

		gfc_line_cpy(btn->name, sj_get_string_value(sj_object_get_value(btnJson, "name")));
		gfc_line_cpy(btn->text, sj_get_string_value(sj_object_get_value(btnJson, "displayName")));
		gfc_line_cpy(btn->command, sj_get_string_value(sj_object_get_value(btnJson, "command")));
		sj_object_get_vector2d(btnJson, "position", &btn->position);

		gfc_list_append(menu->buttons, btn);
		menu->item_max++;
	}
	
}
*/

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

	closet->skincolors[0] = gf3d_texture_load("models/crops/green.png");

	closet->hair[0] = gf3d_mesh_load("models/character/hair1.obj");
	closet->hair[1] = gf3d_mesh_load("models/character/hair2.obj");

	closet->haircolors[0] = gf3d_texture_load("models/crops/green.png");

	closet->tops[0] = gf3d_mesh_load("models/character/shirt1.obj");
	closet->tops[1] = gf3d_mesh_load("models/character/shirt2.obj");

	closet->topcolors[0] = gf3d_texture_load("images/default.png");

	closet->shoes[0] = gf3d_mesh_load("models/character/shoes.obj");

	closet->shoecolors[0] = gf3d_texture_load("images/default.png");
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
	if (strcmp(category, "shoe") == 0)
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
	gf3d_mesh_draw(closet->shoes[a->currentShoes], modelMat, GFC_COLOR_WHITE, closet->shoecolors[a->currentShoecolor], lightPos, lightColor);
}

CharacterAppearance* character_creator_appearance_new()
{
	CharacterAppearance* a;
	a = gfc_allocate_array(sizeof(CharacterAppearance), 1);
	
	// default items
	a->currentHair = 1;
	a->currentHaircolor = 0;
	a->currentTop = 0;
	a->currentTopcolor = 0;
	a->currentBottomcolor = 0;
	a->currentShoes = 0;
	a->currentShoecolor = 0;

	return a;
}