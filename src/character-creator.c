#include "simple_logger.h"
#include "simple_json.h"

#include "character_creator.h"

CharacterAppearance* a = NULL;

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

CharacterAppearance* character_update_appearance()
{
	return a;
}

void character_change_appearance(const char* category)
{
	if (strcmp(category, "skin") == 0)
	{
		a->currentSkincolor = (a->currentSkincolor + 1) % (sizeof(a->skincolors) / sizeof(a->skincolors[0]));
	}
	if (strcmp(category, "hair") == 0)
	{
		a->currentHair = (a->currentHair + 1) % (sizeof(a->hair) / sizeof(a->hair[0]));
	}
	if (strcmp(category, "haircolor") == 0)
	{
		a->currentHaircolor = (a->currentHaircolor + 1) % (sizeof(a->haircolors) / sizeof(a->haircolors[0]));
	}
	if (strcmp(category, "top") == 0)
	{
		a->currentTop = (a->currentTop + 1) % (sizeof(a->tops) / sizeof(a->tops[0]));
	}
	if (strcmp(category, "topcolor") == 0)
	{
		a->currentTopcolor = (a->currentTopcolor + 1) % (sizeof(a->topcolors) / sizeof(a->topcolors[0]));
	}
	if (strcmp(category, "shoe") == 0)
	{
		a->currentShoes = (a->currentShoes + 1) % (sizeof(a->shoes) / sizeof(a->shoes[0]));
	}
	if (strcmp(category, "shoecolor") == 0)
	{
		a->currentShoecolor = (a->currentShoecolor + 1) % (sizeof(a->shoecolors) / sizeof(a->shoecolors[0]));
	}

}

CharacterAppearance* character_appearance_load() 
{ 
	a = gfc_allocate_array(sizeof(CharacterAppearance), 1);
	if (!a) {
		slog("failed to allocate appearance");
		return;
	}
	
	a->body = gf3d_mesh_load("models/character/body.obj");

	a->skincolors[0] = gf3d_texture_load("models/crops/green.png");

	a->hair[0] = gf3d_mesh_load("models/character/hair1.obj");
	a->hair[1] = gf3d_mesh_load("models/character/hair2.obj");

	a->haircolors[0] = gf3d_texture_load("models/crops/green.png");

	a->tops[0] = gf3d_mesh_load("models/character/shirt1.obj");
	a->tops[1] = gf3d_mesh_load("models/character/shirt2.obj");

	a->topcolors[0] = gf3d_texture_load("images/default.png");

	a->shoes[0] = gf3d_mesh_load("models/character/shoes.obj");

	a->shoecolors[0] = gf3d_texture_load("images/default.png");

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

void character_preview_draw(GFC_Vector3D lightPos, GFC_Color lightColor)
{
	if (!a) return;
	//slog("drawing happening");
	GFC_Matrix4 modelMat;
	
	gfc_matrix4_from_vectors(
		modelMat,
		gfc_vector3d(10,0,-10),
		gfc_vector3d(0, 0, 3),
		gfc_vector3d(1.5, 1.5, 1.5));

	gf3d_mesh_draw(a->body, modelMat, GFC_COLOR_WHITE, a->skincolors[a->currentSkincolor], lightPos, lightColor);
	gf3d_mesh_draw(a->hair[a->currentHair], modelMat, GFC_COLOR_WHITE, a->haircolors[a->currentHaircolor], lightPos, lightColor);
	gf3d_mesh_draw(a->tops[a->currentTop], modelMat, GFC_COLOR_WHITE, a->topcolors[a->currentTopcolor], lightPos, lightColor);
	gf3d_mesh_draw(a->shoes[a->currentShoes], modelMat, GFC_COLOR_WHITE, a->shoecolors[a->currentShoecolor], lightPos, lightColor);
}

CharacterPreview* character_preview_new()
{
	CharacterPreview* preview;
	preview = gfc_allocate_array(sizeof(CharacterPreview), 1);
}