#include "simple_logger.h"
#include "simple_json.h"

#include "menu.h"

#include "npc.h"

static SJson* npcDefs = NULL;

void npc_free()
{
	if (!npcDefs) return;
	npcDefs = NULL;
}

void npc_think(Entity* self)
{
	NPCEntityData* data;
	Entity* monster = monster_get_the();
	if ((!self) || (!self->data) || (!monster)) return;
	data = self->data;

	if (gfc_vector3d_magnitude_between(monster->position, self->position) < 15)
	{
		if (gfc_input_command_released("select"))
		{	
			if (data->dialogue_box->visible == 0)
			{
				menu_set_game_state(GS_Pause);
				data->dialogue_box->visible = 1;
			}
			else if (!data->spokenTo)
			{
				menu_set_game_state(GS_Pause);
				gfc_line_cpy(data->dialogue_box->text, data->dialogue2);
				data->spokenTo = 1;
			}
			else
			{
				data->dialogue_box->visible = 0;
			}
		}
	}
}

void npc_draw(Entity* self, GFC_Vector3D lightPos, GFC_Color lightColor)
{
	GFC_Matrix4 modelMat;
	CharacterCloset* closet;
	CharacterAppearance* a;
	NPCEntityData* data;
	if ((!self) || (!self->data))
	return;
	data = self->data;
	a = data->appearance;
	closet = character_creator_get_closet();
	if ((!a) || (!closet)) return;

	gfc_matrix4_from_vectors(
		modelMat,
		self->position,
		self->rotation,
		self->scale);

	gf3d_mesh_draw(closet->body, modelMat, GFC_COLOR_WHITE, closet->skincolors[a->currentSkincolor], lightPos, lightColor);
	gf3d_mesh_draw(closet->hair[a->currentHair], modelMat, GFC_COLOR_WHITE, closet->haircolors[a->currentHaircolor], lightPos, lightColor);
	gf3d_mesh_draw(closet->tops[a->currentTop], modelMat, GFC_COLOR_WHITE, closet->topcolors[a->currentTopcolor], lightPos, lightColor);
	gf3d_mesh_draw(closet->bottoms[a->currentBottom], modelMat, GFC_COLOR_WHITE, closet->bottomcolors[a->currentBottomcolor], lightPos, lightColor);
	gf3d_mesh_draw(closet->shoes[a->currentShoes], modelMat, GFC_COLOR_WHITE, closet->shoecolors[a->currentShoecolor], lightPos, lightColor);
}

void npc_dialogue_box_think(UI* ui)
{
	if ((gfc_input_command_pressed("select")))
	{
		menu_set_game_state(GS_Play);
	}
}

void npc_dialogue_box_draw(UI* ui)
{
	if (!ui) return;
	GFC_Vector2D text_pos = { 0 };
	//GFC_Rect bounds;

	gf2d_font_draw_line_tag(ui->title, FT_H2, GFC_COLOR_YELLOW, ui->title_position);
	gfc_vector2d_add(text_pos, ui->title_position, gfc_vector2d(0,40));
	gf2d_font_draw_line_tag(ui->text, FT_Normal, GFC_COLOR_WHITE, text_pos);

	// text wrapping has no position? 
	//bounds = gf2d_font_get_text_wrap_bounds_tag(ui->text, FT_Normal,700,190);
	//gf2d_font_draw_text_wrap_tag(ui->text, FT_Normal, GFC_COLOR_WHITE, bounds);	
}

void npc_init(const char* filename)
{
	SJson* json;
	json = sj_load(filename);
	npcDefs = sj_object_get_value(json, "npc");
	if (!npcDefs)
	{
		slog("npc definition file %s does not contain npc list", filename);
		sj_free(json);
		json = NULL;
		return;
	}

	atexit(npc_free);
}

SJson* npc_get_def_by_name(const char* name)
{
	int i, c;
	SJson* npc;
	const char* npcName = NULL;
	if (!name) return NULL;
	if (!npcDefs)
	{
		slog("no npc definitions loaded");
		return NULL;
	}
	c = sj_array_get_count(npcDefs);
	for (int i = 0; i < c; i++)
	{
		npc = sj_array_get_nth(npcDefs, i);
		if (!npc) continue;
		npcName = sj_object_get_value_as_string(npc, "name");
		if (!npcName) continue;
		if (gfc_strlcmp(name, npcName) == 0) {
			//found it
			return npc;
		}
	}
	slog("no npc found by name", name);
	return NULL;
}

Entity* npc_spawn(const char* name)
{
	SJson* def;
	Entity* self;
	NPCEntityData* data;
	self = entity_new();
	if (!self) return NULL;
	data = gfc_allocate_array(sizeof(NPCEntityData), 1);
	if (!data)
	{
		free(self);
		return NULL;
	}
	self->data = data;

	def = npc_get_def_by_name(name);

	//populate data
	gfc_line_cpy(self->name, sj_object_get_value_as_string(def, "name"));
	gfc_line_cpy(self->displayName, sj_object_get_value_as_string(def, "displayName"));
	self->entityType = "npc";
	sj_object_get_vector3d(def, "position", &self->position);
	self->color = GFC_COLOR_WHITE;
	self->rotation = gfc_vector3d(0, 0, 0);
	self->velocity = gfc_vector3d(0, 0, 0);
	self->collisionRadius = 2;
	sj_object_get_value_as_int(def, "skin", &data->skin);
	sj_object_get_vector4d(def, "clothing", &data->clothing);
	sj_object_get_vector4d(def, "colors", &data->colors);
	gfc_line_cpy(data->dialogue1, sj_object_get_value_as_string(def, "dialogue1"));
	gfc_line_cpy(data->dialogue2, sj_object_get_value_as_string(def, "dialogue2"));
	data->spokenTo = 0;

	data->appearance = character_creator_appearance_new();
	if (!data->appearance) return;
	data->appearance = character_creator_change_appearance_all(data->appearance, data->skin, data->clothing, data->colors);

	data->dialogue_box = ui_new();
	if (!data->dialogue_box) return;
	gfc_line_cpy(data->dialogue_box->name, "dialogue box");
	gfc_line_cpy(data->dialogue_box->title, self->displayName);
	data->dialogue_box->title_position = gfc_vector2d(280, 500);
	data->dialogue_box->background = gf2d_sprite_load_image("images/ui/dialogue_box.png");
	data->dialogue_box->bg_position = gfc_vector2d(250, 470);
	gfc_line_cpy(data->dialogue_box->text, data->dialogue1);
	data->dialogue_box->visible = 0;
	data->dialogue_box->draw = npc_dialogue_box_draw;
	data->dialogue_box->think = npc_dialogue_box_think;
	
	self->think = npc_think;
	self->draw = npc_draw;
	self->free = npc_free;

	slog("NPC spawned: %s", self->displayName);
	return self;
}