#include <SDL.h>            

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

#include "entity.h"
#include "monster.h"

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
    Uint32 time;
    Mesh* mesh;
    Entity* monster;
    Texture* texture;
    GFC_Vector3D lightPos = { 5,5,50 };
    GFC_Vector3D cam = { 0,50,0 };
    GFC_Matrix4 id;
    GFC_Matrix4 dinoM;
    float theta = 0;
    //initializtion    
    parse_arguments(argc, argv);
    init_logger("gf3d.log", 0);
    slog("gf3d begin");
    time = SDL_GetTicks();
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);
    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg"); 
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000);

    entity_system_init(8000);
    slog("time to init: %i ms", SDL_GetTicks() - time);

    //game init
    srand(SDL_GetTicks());
    slog_sync();
    gf2d_mouse_load("actors/mouse.actor");
    mesh = gf3d_mesh_load("models/dino/dino.obj");
    texture = gf3d_texture_load("models/dino/dino.png");
    //mesh = gf3d_mesh_load("models/sky/sky.obj");
    //texture = gf3d_texture_load("models/sky/sky.png");
    gfc_matrix4_identity(id);

    gf3d_camera_look_at(gfc_vector3d(0, 0, 0), &cam);
    //for (int i = 0; i < 500; i++)
    //{
    //    monster_spawn(gfc_crandom()*50, gfc_crandom*50, gfc_color(gfc_random(), gfc_random(), gfc_random(),1));
    //}
    monster = monster_spawn(gfc_vector3d(0, 0, 0), GFC_COLOR_WHITE);
    //main game loop
    while (!_done)
    {
        slog("Game loop:");
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        //world updates
        theta += 0.1;
        gfc_matrix4_rotate_z(dinoM, id, theta);
        entity_system_think_all();
        entity_system_update_all();
        //camera updaes
        gf3d_camera_update_view();
        gf3d_vgraphics_render_start();
            //3D draws
            //gf3d_
        slog("Before draw all");
            entity_system_draw_all(gfc_vector3d(0,0,0), GFC_COLOR_LIGHTCYAN);
            slog("after draw all");

           // gf3d_mesh_draw(mesh, dinoM, GFC_COLOR_WHITE, texture, lightPos, GFC_COLOR_RED);
            //2D draws
            gf2d_font_draw_line_tag("ALT+F4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));
            gf2d_mouse_draw();
        gf3d_vgraphics_render_end();
        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());
    //cleanup
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