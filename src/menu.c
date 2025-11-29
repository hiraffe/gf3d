#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_input.h"
#include "gfc_audio.h"

#include "menu.h"

static SJson* menuDefs = NULL;
static SJson* buttonDefs = NULL;
Mix_Music *menuMusic = NULL, *music = NULL;
Mix_Chunk *selected = NULL, *click = NULL;

GameState game_state = GS_MainMenu;

void menu_free()
{
	if (!menuDefs) return;
	menuDefs = NULL;

	Mix_FreeMusic(menuMusic);
	Mix_FreeMusic(music);
	Mix_FreeChunk(selected);
	Mix_FreeChunk(click);
}

void menu_open(UI* ui)
{
	if (!ui) return;
	if (ui->visible == 1) return;
	ui->visible = 1;
	ui->item_selected = 0;
}

void menu_close(UI* ui)
{
	if (!ui) return;
	if (ui->visible == 0) return;
	ui->visible = 0;
}

GameState menu_get_game_state()
{
	return game_state;
}

void menu_set_game_state(GameState g)
{
	game_state = g;
}

void menu_play_selected()
{
	Mix_PlayChannel(0, selected, 0);
}

void menu_play_click()
{
	Mix_PlayChannel(0, click, 0);
}

GameState menu_execute_command(UI* self)
{
	Menu_Button* btn;
	const char* cmd;

	btn = gfc_list_get_nth(self->buttons, self->item_selected);
	cmd = btn->command;
	
	if (strcmp(cmd, "start-game") == 0)
	{
		menu_close(self);
		Mix_HaltMusic;
		Mix_PlayMusic(music, -1);
		return GS_Play;
	}
	if (strcmp(cmd, "resume-game") == 0)
	{
		Mix_ResumeMusic();
		menu_close(self);
		return GS_Play;
	}
	if (strcmp(cmd, "editor") == 0)
	{
		menu_close(self);
		return GS_CharacterCreator;
	}
	if (strcmp(cmd, "end-game") == 0)
	{
		return GS_Quit;
	}

	slog("Unknown UI command: %s", cmd);
}

void menu_think(UI* self)
{
	//scroll up and down
	if (gfc_input_command_pressed("pandown"))
	{
		self->item_selected = (self->item_selected + 1) % self->item_max;
		menu_play_selected();
	}
	if (gfc_input_command_pressed("panup"))
	{
		self->item_selected = (self->item_selected - 1 + self->item_max) % self->item_max;  // prevents negatives
		menu_play_selected();
	}

	//handle commands
	if (gfc_input_command_pressed("enter"))
	{
		menu_play_click();
		game_state = menu_execute_command(self);
	}
}

void menu_draw(UI* ui)
{
	Menu_Button* btn;
	GFC_Color color;
	int offset = 0;

	gf2d_font_draw_line_tag(ui->title, FT_H2, GFC_COLOR_YELLOW, ui->title_position);

	for (int i = 0; i < ui->buttons->count; i++)
	{
		btn = gfc_list_get_nth(ui->buttons, i);
		color = (i == ui->item_selected) ? GFC_COLOR_WHITE : GFC_COLOR_YELLOW;
		gf2d_font_draw_line_tag(btn->text, FT_H5, color, btn->position);
		offset += 30;
	}
}

void menu_init(const char* filename)
{
	SJson* json;
	json = sj_load(filename);
	menuDefs = sj_object_get_value(json, "menu");
	if (!menuDefs)
	{
		slog("menu definition file %s does not contain menu list", filename);
		sj_free(json);
		json = NULL;
		return;
	}

	menuMusic = gfc_sound_load_music("audio/monkeys-spinning-monkeys-kevin-macleod-main-version-8413-02-05.mp3");
	music = gfc_sound_load_music("audio/a-day-in-my-life-dark-cat-main-version-32322-02-31.mp3");
	selected = Mix_LoadWAV("audio/effects/selected.wav");
	click = Mix_LoadWAV("audio/effects/click.wav");

	atexit(menu_free);
}

SJson* menu_get_def_by_name(const char* name)
{
	int i, c;
	SJson* menu;
	const char* menuName = NULL;
	if (!name) return NULL;
	if (!menuDefs)
	{
		slog("no menu definitions loaded");
		return NULL;
	}
	c = sj_array_get_count(menuDefs);
	for (int i = 0; i < c; i++)
	{
		menu = sj_array_get_nth(menuDefs, i);
		if (!menu) continue;
		menuName = sj_object_get_value_as_string(menu, "name");
		if (!menuName) continue;
		if (gfc_strlcmp(name, menuName) == 0) {
			//found it
			return menu;
		}
	}
	slog("no menu found by name", name);
	return NULL;
}

void menu_button_add(UI* ui, const char* name, const char* text, const char* cmd, GFC_Vector2D pos)
{
	Menu_Button* btn = gfc_allocate_array(sizeof(Menu_Button), 1);

	gfc_line_cpy(btn->name, name);
	gfc_line_cpy(btn->text, text);
	gfc_line_cpy(btn->command, cmd);
	btn->position = pos;

	// set button position, size, hitbox, etc.
	gfc_list_append(ui->buttons, btn);
	ui->item_max++;
}

SJson* menu_get_buttons(UI* menu, SJson* json)
{
	SJson* buttons = sj_object_get_value(json, "buttons");
	int buttonCount = sj_array_get_count(buttons);
	GFC_Vector2D pos = { 0 };

	for (int i = 0; i < buttonCount; i++)
	{
		SJson* btnJson = sj_array_get_nth(buttons, i);

		const char* name = sj_get_string_value(sj_object_get_value(btnJson, "name"));
		const char* display = sj_get_string_value(sj_object_get_value(btnJson, "displayName"));
		const char* command = sj_get_string_value(sj_object_get_value(btnJson, "command"));
		
		Menu_Button* btn = gfc_allocate_array(sizeof(Menu_Button), 1);

		gfc_line_cpy(btn->name, name);
		gfc_line_cpy(btn->text, display);
		gfc_line_cpy(btn->command, command);
		sj_object_get_vector2d(btnJson, "position", &btn->position);

		// set button position, size, hitbox, etc.
		gfc_list_append(menu->buttons, btn);
		menu->item_max++;
	}
}

UI* menu_new(const char* name)
{
	SJson* def;
	UI* self;
	self = ui_new();
	if (!self) return NULL;

	def = menu_get_def_by_name(name);

	//populate data
	gfc_line_cpy(self->name, name);
	gfc_line_cpy(self->title, sj_object_get_value_as_string(def, "title"));
	sj_object_get_vector2d(def, "title_position", &self->title_position);
	gfc_line_cpy(self->type, "menu");
	self->background = gf2d_sprite_load_image(sj_object_get_value_as_string(def, "background"));
	sj_object_get_vector2d(def, "bg_position", &self->bg_position);
	self->item_max = 0;

	self->think = menu_think;
	self->draw = menu_draw;
	self->free = menu_free;

	menu_get_buttons(self, def);

	slog("%s created", self->name);
	return self;
}
