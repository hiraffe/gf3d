#include "simple_logger.h"

#include "space.h"


Space* space_new()
{
	Space* space;
	space = gfc_allocate_array(sizeof(Space), 1);
	if (!space) return NULL;

	space->staticMeshes = gfc_list_new();

	space->iterations = 1;
	space->step = 1;

	return space;
}

void space_set_iterations(Space *space, Uint32 iterations)
{
	if (iterations == 0)
	{
		slog("cannot make a space have 0 iterations");
		return;
	}
	space->iterations = iterations;
	space->step = 1 / iterations;
}

space_step_body(Space* space, Body* body)
{
	//
}

space_step(Space *space)
{
	//
}

void space_run(Space* space)
{
	int i,c;
	if (!space) return;

	c = gfc_list_count(space->bodies);
	for (i = 0; i < c; i++)
	{
		body_reset_for_updates(gfc_list_nth(space->bodies, i), space->step);
	}
	for (i = 0; i < space->iterations; i++)
	{
		space_step(space);
	}
}

void space_add_body(Space* space, Body* body)
{
	gfc_list_append(space->bodies, body);
}