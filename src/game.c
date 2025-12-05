#include <SDL.h>    //Precompiled        

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_config_def.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"
#include "gfc_string.h"
#include "gfc_actions.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"

#include "gf3d_camera.h"
#include "gf3d_mesh.h"

#include "world.h"
#include "ui.h"
#include "entity.h"
#include "monster.h"
#include "camera_entity.h"
#include "crop.h"
#include "animal.h"
#include "item.h"
#include "inventory.h"
#include "shop.h"
#include "shop_menu.h"
#include "hotbar.h"
#include "menu.h"
#include "character_creator.h"

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

void parse_arguments(int argc, char* argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}


int main(int argc, char* argv[])
{
    //local variables
    GameState game_state;
    World* world;
    Entity* monster;
    Entity *shop, *shop2;
    UI *startMenu, *pauseMenu, *editorMenu;
    UI* hotbar;
    float theta = 0;
    GFC_Vector3D lightPos = { 5,5,20 };
    GFC_Vector3D cam = { 0,50,0 };
    Entity* camEnt;
    Mix_Music* menuMusic;
    //initializtion    
    parse_arguments(argc, argv);
    init_logger("gf3d.log", 0); //1 wont delete log file at end
    slog("gf3d begin");
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);
    gfc_audio_init(1024, 1, 0);
    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000);
    //entity init
    entity_system_init(1024); 
    ui_manager_init(16);
    menu_init("defs/menu.def");
    crops_init("defs/crops.def");
    animals_init("defs/animals.def");
    items_init("defs/items.def");

    //game init
    srand(SDL_GetTicks());
    slog_sync();
    gf2d_mouse_load("actors/mouse.actor");

    monster = monster_spawn(gfc_vector3d(0, 0, 0), GFC_COLOR_ORANGE);
    camEnt = camera_entity_spawn(gfc_vector3d(0, 50, -5), monster);
    monster_set_camera_ent(monster, camEnt);

    startMenu = menu_new("start-menu");
    pauseMenu = menu_new("pause-menu");
    editorMenu = menu_new("character-creator");
    menu_open(startMenu);
    menuMusic = gfc_sound_load_music("audio/monkeys-spinning-monkeys-kevin-macleod-main-version-8413-02-05.mp3");

    game_state = menu_get_game_state();

    /*
    while (game_state == GS_MainMenu)
    {
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();

        if (!Mix_PlayingMusic()) Mix_PlayMusic(menuMusic, 1);
        ui_manager_think_all();
        gf3d_vgraphics_render_start();
        ui_manager_draw_all();
        gf2d_mouse_draw();
        gf3d_vgraphics_render_end();
        game_state = menu_get_game_state();
    }
    Mix_PauseMusic();
    slog("loading");
    */

    world = world_load("defs/terrain.def");
    shop = shop_spawn(gfc_vector3d(-50,-50,6), "Scary Seeds");
    shop2 = shop_spawn(gfc_vector3d(-70,-50,6), "Terrrifying Tools");
    hotbar = hotbar_new();
    hotbar_open(hotbar);
    
    //gfc_matrix4_identity(id);
    //gf3d_camera_look_at(gfc_vector3d(0, 0, 0), &cam);
    
    // main game loop 
    while (!_done)
    {
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        
        switch (game_state) {
            case GS_MainMenu:
                if(!Mix_PlayingMusic()) Mix_PlayMusic(menuMusic, 1);
                ui_manager_think_all();
                gf3d_vgraphics_render_start();
                ui_manager_draw_all();
                gf2d_mouse_draw();
                gf3d_vgraphics_render_end();
                break;
            case GS_Pause:
                Mix_PauseMusic();
                ui_manager_think_all();
                gf3d_vgraphics_render_start();
                entity_system_draw_all(lightPos, GFC_COLOR_WHITE);
                world_draw(world);
                ui_manager_draw_all();
                gf2d_mouse_draw();
                gf3d_vgraphics_render_end();
                break;
            case GS_CharacterCreator:
                menu_open(editorMenu);
                // just update the preview entity
                ui_manager_think_all();
                gf3d_camera_look_at(gfc_vector3d(0, 0, 0), &cam);
                gf3d_camera_update_view();
                gf3d_vgraphics_render_start();
                // draw preview entity
                character_preview_draw(lightPos, GFC_COLOR_WHITE);
                ui_manager_draw_all();
                gf3d_vgraphics_render_end();
                break;
            case GS_Play:
                //world updates
                entity_system_think_all();
                entity_system_update_all();
                ui_manager_think_all();
                //camera updates
                gf3d_camera_update_view();
                gf3d_vgraphics_render_start();
                //3D draws
                entity_system_draw_all(lightPos, GFC_COLOR_WHITE);
                world_draw(world);
                //2D draws
                ui_manager_draw_all();
                gf3d_vgraphics_render_end();
                if (gfc_input_command_pressed("cancel"))
                {
                    menu_set_game_state(GS_Pause);
                    menu_open(pauseMenu);
                }
                break;
            case GS_Quit:
                _done = 1;
                break;
        }
        game_state = menu_get_game_state();
        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());
    //cleanup
    Mix_FreeMusic(menuMusic);
    slog("gf3d program end");
    exit(0);
    slog_sync();
    return 0;
}

void parse_arguments(int argc, char* argv[])
{
    int a;

    for (a = 1; a < argc; a++)
    {
        if (strcmp(argv[a], "--debug") == 0)
        {
            __DEBUG = 1;
        }
    }
}

void game_frame_delay()
{
    Uint32 diff;
    static Uint32 now;
    static Uint32 then;
    then = now;
    slog_sync();// make sure logs get written when we have time to write it
    now = SDL_GetTicks();
    diff = (now - then);
    if (diff < frame_delay)
    {
        SDL_Delay(frame_delay - diff);
    }
    fps = 1000.0 / MAX(SDL_GetTicks() - then, 0.001);
    //     slog("fps: %f",fps);
}
/*eol@eof*/