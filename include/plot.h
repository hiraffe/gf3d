#ifndef __PLOT_H_
#define __PLOT_H_

#include "entity.h"

typedef struct
{
	const char* size;
	Mesh* long_fence;
	Mesh* short_fence;
	Mesh* sideways_fence;
	GFC_Vector3D frontL_pos;
	GFC_Vector3D frontR_pos;
	GFC_Vector3D sideL_pos;
	GFC_Vector3D sideR_pos;
	GFC_Vector3D back_pos;
	Entity* door;
}PlotEntityData;

Entity* plot_spawn(GFC_Vector3D position, const char* size);

#endif