#include "simple_logger.h"
#include "simple_json.h"

#include "gf3d_buffers.h"
#include "gf3d_swapchain.h"
#include "gf3d_obj_load.h"
#include "gf3d_vgraphics.h"
#include "gf3d_camera.h"

#include "gf3d_mesh.h"

#define MESH_ATTRIBUTE_COUNT 3

extern int __DEBUG;

typedef struct
{
	Mesh* mesh_list;
	Uint32 mesh_count;
	Uint32 mesh_max;
	Uint32 chain_length;
	VkDevice device;
	Pipeline* pipe;
	VkVertexInputAttributeDescription   attributeDescriptions[MESH_ATTRIBUTE_COUNT];
	VkVertexInputBindingDescription     bindingDescription;
	Texture*							defaultTexture;
}MeshManager;

static MeshManager mesh_manager = { 0 };

void gf3d_mesh_manager_close()
{
	int i;
	for (i = 0; i < mesh_manager.mesh_max; i++)
	{
		if (mesh_manager.mesh_list[i]._refCount > 0)
		{
			gf3d_mesh_delete(&mesh_manager.mesh_list[i]);
		}
	}
	if (mesh_manager.mesh_list)
	{
		free(mesh_manager.mesh_list);
		mesh_manager.mesh_list = NULL;
	}
	/*
	if (mesh_manager.faceBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(mesh_manager.device, mesh_manager.faceBuffer, NULL);
	}
	if (mesh_manager.faceBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(mesh_manager.device, mesh_manager.faceBufferMemory, NULL);
	}
	*/
	memset(&mesh_manager, 0, sizeof(MeshManager));
	if (__DEBUG)slog("mesh manager closed");
}

void gf3d_mesh_init(Uint32 mesh_max)
{
	Uint32 count = 0;

	if (mesh_max == 0)
	{
		slog("cannot intilize mesh manager for 0 mesh");
		return;
	}
	mesh_manager.chain_length = gf3d_swapchain_get_chain_length();
	mesh_manager.mesh_list = (Mesh*)gfc_allocate_array(sizeof(Mesh), mesh_max);
	if (!mesh_manager.mesh_list)
	{
		slog("failed to allocate mesh_list");
		return;
	}
	mesh_manager.mesh_max = mesh_max;
	mesh_manager.mesh_count = 0;
	mesh_manager.device = gf3d_vgraphics_get_default_logical_device();

	mesh_manager.defaultTexture = gf3d_texture_load("images/default.png");

	gf3d_mesh_get_attribute_descriptions(&count);
	mesh_manager.pipe = gf3d_pipeline_create_from_config(
		gf3d_vgraphics_get_default_logical_device(),
		"config/overlay_pipeline.cfg",
		gf3d_vgraphics_get_view_extent(),
		mesh_max,
		gf3d_mesh_get_bind_description(),
		gf3d_mesh_get_attribute_descriptions(NULL),
		count,
		sizeof(MeshUBO),
		VK_INDEX_TYPE_UINT16
	);

	if (__DEBUG)slog("mesh manager initiliazed");
	atexit(gf3d_mesh_manager_close);
}

Mesh* gf3d_mesh_get_by_filename(const char* filename)
{
	int i;
	if (!filename) return NULL;
	for (i = 0; i < mesh_manager.mesh_max; i++)
	{
		if (! &mesh_manager.mesh_list[i]) continue;
		if (! mesh_manager.mesh_list[i]._refCount) continue;
		if (gfc_strlcmp(mesh_manager.mesh_list[i].filename, filename) == 0)
		{
			return &mesh_manager.mesh_list[i];
		}
	}
	return NULL;
}

MeshPrimitive* gf3d_mesh_primitive_new()
{
	MeshPrimitive *prim =  gfc_allocate_array(sizeof(MeshPrimitive), 1);
	if (!prim) slog("failed to allocate MeshPrimitive");
	return prim;
}


Mesh* gf3d_mesh_new()
{
	int i;
	for (i = 0; i < mesh_manager.mesh_max; i++)
	{
		if (mesh_manager.mesh_list[i]._refCount == 0)
		{
			memset(&mesh_manager.mesh_list[i], 0, sizeof(Mesh));
			mesh_manager.mesh_list[i]._refCount = 1;
			mesh_manager.mesh_list[i].primitives = NULL;
			mesh_manager.mesh_count++;
			return &mesh_manager.mesh_list[i];
		}
	}
	slog("no free mesh slots available (max %u)", mesh_manager.mesh_max);
	return NULL;
}

Mesh* gf3d_mesh_load(const char *filename)
{
	Mesh* mesh;
	ObjData *obj;
	MeshPrimitive* primitive;

	if (!filename)return NULL;
	mesh = gf3d_mesh_get_by_filename(filename);
	if (mesh) return mesh;

	obj = gf3d_obj_load_from_file(filename);
	if (!obj)
	{
		slog("failed to load obj file %s", filename);
		return NULL;
	}
	mesh = gf3d_mesh_new();
	if (!mesh)
	{
		gf3d_obj_free(obj);
		return NULL;
	}
	primitive = gf3d_mesh_primitive_new();
	if (!primitive)
	{
		slog("failed to allocate mesh prinmitivr for file %s", filename);
		gf3d_obj_free(obj);
		gf3d_mesh_free(mesh);
		return NULL;
	}

	gfc_list_append(mesh->primitives,primitive);
	primitive->objData = obj;

	//figure this out
	//gf3d_mesh_primitive_create_vertex_buffers(primitive);
	//gf3d_mesh_setup_face_buffers(primitive);

	return mesh;
}

void gf3d_mesh_free(Mesh* mesh)
{
	if (!mesh)return;
	mesh->_refCount--;
	if (mesh->_refCount <= 0) gf3d_mesh_delete(mesh);
}

void gf3d_mesh_delete(Mesh* mesh)
{
	int i, c;
	MeshPrimitive* prim;
	if (!mesh) return;
	if (mesh->primitives)
	{
		c = gfc_list_get_count(mesh->primitives);
		for (i = 0; i < c; i++)
		{
			MeshPrimitive* prim = (MeshPrimitive*)gfc_list_nth(mesh->primitives, i);
			if (!prim) continue;

			if (prim->vertexBuffer != VK_NULL_HANDLE)
			{
				vkDestroyBuffer(mesh_manager.device, prim->vertexBuffer, NULL);
				prim->vertexBuffer = VK_NULL_HANDLE;
			}
			if (prim->vertexBufferMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(mesh_manager.device, prim->vertexBufferMemory, NULL);
				prim->vertexBufferMemory = VK_NULL_HANDLE;
			}
			if (prim->faceBuffer != VK_NULL_HANDLE)
			{
				vkDestroyBuffer(mesh_manager.device, prim->faceBuffer, NULL);
				prim->faceBuffer = VK_NULL_HANDLE;
			}
			if (prim->faceBufferMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(mesh_manager.device, prim->faceBufferMemory, NULL);
				prim->faceBufferMemory = VK_NULL_HANDLE;
			}

			if (prim->objData)
			{
				gf3d_obj_free(prim->objData);
				prim->objData = NULL;
			}

			free(prim);
		}
		mesh->primitives = NULL;
	}
	mesh->_refCount = 0;
	mesh_manager.mesh_count = (mesh_manager.mesh_count > 0) ? mesh_manager.mesh_count - 1 : 0;
}

void gf3d_mesh_create_buffers(MeshPrimitive* prim, Face *faces, Uint32 fcount)
{
	void* data = NULL;
	VkDevice device = gf3d_vgraphics_get_default_logical_device();
	VkDeviceSize bufferSize = sizeof(Face) * fcount;
	Vertex* vertices;
	Uint32 vcount;
	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

	if ((!prim)||(!prim->objData)) return;
	if ((!faces) || (!fcount))
	{
		faces = prim->objData->outFace;
		fcount = prim->objData->face_count;
	}
	if ((!faces) || (!fcount)) return;

	vertices = prim->objData->faceVertices; //
	vcount = prim->objData->face_vert_count; //

	gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, faces, (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->faceBuffer, &prim->faceBufferMemory);

	gf3d_buffer_copy(stagingBuffer, prim->faceBuffer, bufferSize);

	prim->faceCount = fcount;
	vkDestroyBuffer(device, stagingBuffer, NULL);
	vkFreeMemory(device, stagingBufferMemory, NULL);
}

void gf3d_mesh_primitive_vertex_buffers(MeshPrimitive* prim, Face* faces, Uint32 fcount) {}

void gf3d_mesh_primitive_queue_render(MeshPrimitive *prim, Pipeline* pipe, void* uboData, Texture* texture)
{
	if ((!prim) || (!pipe) || (!uboData))return;
	if (!texture) texture = mesh_manager.defaultTexture;
	gf3d_pipeline_queue_render(
		pipe,
		prim->vertexBuffer,
		prim->vertexCount,
		prim->faceBuffer,
		uboData,
		texture);
}

void gf3d_mesh_queue_render(Mesh *mesh, Pipeline *pipe, void *uboData, Texture *texture)
{
	int i, c;
	MeshPrimitive* prim;
	if ((!mesh) || (!pipe) || (!uboData))return;
	c = gfc_list_count(mesh->primitives);
	for (i = 0; i < c; i++)
	{
		prim = gfc_list_nth(mesh->primitives, i);
		if (!prim)continue;
		gf3d_mesh_primitive_queue_render(prim, pipe, uboData, texture);
	}
}

void gf3d_mesh_draw(Mesh* mesh, GFC_Matrix4 modelMat, GFC_Color mod, Texture* texture)
{
	MeshUBO ubo = { 0 };
	if (!mesh)return;

	gfc_matrix4_copy(ubo.model, modelMat);
	gf3d_vgraphics_get_view(&ubo.view);
	gf3d_vgraphics_get_projection_matrix(&ubo.proj);

	ubo.color = gfc_color_to_vector4(mod);
	//TODO GFC_Vector4D camera
	//ubo.camera = gfc_vector3dw(gf3d_camera_get_position(),1.0);

	gf3d_mesh_queue_render(mesh, mesh_manager.pipe, &ubo, texture);
}

MeshUBO gf3d_mesh_get_ubo(GFC_Matrix4 modelMat, GFC_Color colorMod)
{
	MeshUBO ubo = { 0 };
	gfc_matrix4_copy(ubo.model, modelMat);
	gf3d_vgraphics_get_view(&ubo.view);
	gf3d_vgraphics_get_projection_matrix(&ubo.proj);
	ubo.color = gfc_color_to_vector4(colorMod);
	return ubo;
}

VkVertexInputBindingDescription* gf3d_mesh_get_bind_description()
{
	mesh_manager.bindingDescription.binding = 0;
	mesh_manager.bindingDescription.stride = sizeof(Vertex);
	mesh_manager.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return &mesh_manager.bindingDescription;
}

VkVertexInputAttributeDescription* gf3d_mesh_get_attribute_descriptions(Uint32* count)
{
	mesh_manager.attributeDescriptions[0].binding = 0;
	mesh_manager.attributeDescriptions[0].location = 0;
	mesh_manager.attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	mesh_manager.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

	mesh_manager.attributeDescriptions[1].binding = 0;
	mesh_manager.attributeDescriptions[1].location = 1;
	mesh_manager.attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	mesh_manager.attributeDescriptions[1].offset = offsetof(Vertex, texel);
	if (count)*count = MESH_ATTRIBUTE_COUNT;
	return mesh_manager.attributeDescriptions;
}

Pipeline* gf3d_mesh_get_pipeline()
{
	return mesh_manager.pipe;
}