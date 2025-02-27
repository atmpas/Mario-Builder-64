#include <PR/ultratypes.h>

#include "sm64.h"
#include "game/ingame_menu.h"
#include "graph_node.h"
#include "behavior_script.h"
#include "behavior_data.h"
#include "game/memory.h"
#include "game/object_helpers.h"
#include "surface_collision.h"
#include "math_util.h"
#include "game/mario.h"
#include "game/object_list_processor.h"
#include "surface_load.h"
#include "game/puppyprint.h"
#include "game/debug.h"

#include "config.h"

/**
 * Partitions for course and object surfaces. The arrays represent
 * the 16x16 cells that each level is split into.
 */
SpatialPartitionCell gStaticSurfacePartition[NUM_CELLS][NUM_CELLS];
SpatialPartitionCell gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS];
SpatialPartitionCell gBlockSurfaces;

/**
 * Pools of data that can contain either surface nodes or surfaces.
 * The static surface pool is resized to be exactly the amount of memory needed for the level geometry.
 * The dynamic surface pool is set at a fixed length and cleared every frame.
 */
struct Surface *gSurfacePool;
struct SurfaceNode *gSurfaceNodePool;

struct Surface *gMainSurfacePool;
struct SurfaceNode *gMainSurfaceNodePool;
struct Surface *gBlockSurfacePool;
struct SurfaceNode *gBlockSurfaceNodePool;

/**
 * Allocate the part of the surface node pool to contain a surface node.
 */
static struct SurfaceNode *alloc_surface_node(u32 dynamic) {
    struct SurfaceNode *node = &gSurfaceNodePool[*gSurfaceNodesAllocated];
    (*gSurfaceNodesAllocated)++;

    node->next = NULL;

    return node;
}

/**
 * Allocate the part of the surface pool to contain a surface and
 * initialize the surface.
 */
struct Surface *alloc_surface(u32 dynamic) {
    struct Surface *surface = &gSurfacePool[*gSurfacesAllocated];
    (*gSurfacesAllocated)++;

    surface->type = SURFACE_DEFAULT;
    surface->object = NULL;

    return surface;
}

/**
 * Iterates through the entire partition, clearing the surfaces.
 */
void clear_spatial_partition(SpatialPartitionCell *cells) {
    register s32 i = sqr(NUM_CELLS);

    while (i--) {
        (*cells)[SPATIAL_PARTITION_FLOORS] = NULL;
        (*cells)[SPATIAL_PARTITION_CEILS] = NULL;
        (*cells)[SPATIAL_PARTITION_WALLS] = NULL;

        cells++;
    }
}

/**
 * Add a surface to the correct cell list of surfaces.
 * @param type 0 = static, 1 = dynamic, 2 = block
 * @param cellX The X position of the cell in which the surface resides
 * @param cellZ The Z position of the cell in which the surface resides
 * @param surface The surface to add
 */
void add_surface_to_cell(s32 type, s32 cellX, s32 cellZ, struct Surface *surface) {
    struct SurfaceNode **list;
    s32 priority;
    s32 sortDir = 1; // highest to lowest, then insertion order (water and floors)
    s32 listIndex;

    Vec3f normal;
    get_surface_normal(normal, surface);

    if (normal[1] > NORMAL_FLOOR_THRESHOLD) {
        listIndex = SPATIAL_PARTITION_FLOORS;
    } else if (normal[1] < NORMAL_CEIL_THRESHOLD) {
        listIndex = SPATIAL_PARTITION_CEILS;
        sortDir = -1; // lowest to highest, then insertion order
    } else {
        listIndex = SPATIAL_PARTITION_WALLS;
        sortDir = 0; // insertion order
    }

    s32 surfacePriority = surface->upperY * sortDir;

    struct SurfaceNode *newNode = alloc_surface_node(type != 0);
    newNode->surface = surface;

    if (type == 1) {
        list = &gDynamicSurfacePartition[cellZ][cellX][listIndex];
    } else if (type == 0) {
        list = &gStaticSurfacePartition[cellZ][cellX][listIndex];
    } else {
        list = &gBlockSurfaces[listIndex];
    }

    if (*list == NULL) {
        *list = newNode;
        return;
    }
    struct SurfaceNode *curNode = *list;

    // Loop until we find the appropriate place for the surface in the list.
    while (curNode->next != NULL) {
        priority = curNode->next->surface->upperY * sortDir;

        if (surfacePriority > priority) {
            break;
        }

        curNode = curNode->next;
    }

    newNode->next = curNode->next;
    curNode->next = newNode;
}

/**
 * Every level is split into CELL_SIZE * CELL_SIZE cells of surfaces (to limit computing
 * time). This function determines the lower cell for a given x/z position.
 * @param coord The coordinate to test
 */
static s32 lower_cell_index(s32 coord) {
    // Move from range [-LEVEL_BOUNDARY_MAX, LEVEL_BOUNDARY_MAX) to [0, 2 * LEVEL_BOUNDARY_MAX)
    coord += LEVEL_BOUNDARY_MAX + 1;

    // [0, NUM_CELLS)
    s32 index = coord / CELL_SIZE;
    // Potentially > NUM_CELLS - 1, but since the upper index is <= NUM_CELLS - 1, not exploitable
    return CLAMP(index, 0, NUM_CELLS - 1);
}

/**
 * Every level is split into CELL_SIZE * CELL_SIZE cells of surfaces (to limit computing
 * time). This function determines the upper cell for a given x/z position.
 * @param coord The coordinate to test
 */
static s32 upper_cell_index(s32 coord) {
    // Move from range [-LEVEL_BOUNDARY_MAX, LEVEL_BOUNDARY_MAX) to [0, 2 * LEVEL_BOUNDARY_MAX)
    coord += LEVEL_BOUNDARY_MAX - 1;

    // [0, NUM_CELLS)
    s32 index = coord / CELL_SIZE;
    // Potentially < 0, but since lower index is >= 0, not exploitable
    return CLAMP(index, 0, NUM_CELLS - 1);
}

/**
 * Every level is split into 16x16 cells, this takes a surface, finds
 * the appropriate cells (with a buffer), and adds the surface to those
 * cells.
 * @param surface The surface to check
 * @param dynamic Boolean determining whether the surface is static or dynamic
 */
void add_surface(struct Surface *surface, s32 dynamic) {
    s32 cellZ, cellX;
    s32 minX, maxX, minZ, maxZ;

    min_max_3i(surface->vertex1[0], surface->vertex2[0], surface->vertex3[0], &minX, &maxX);
    min_max_3i(surface->vertex1[2], surface->vertex2[2], surface->vertex3[2], &minZ, &maxZ);

    s32 minCellX = lower_cell_index(minX);
    s32 maxCellX = MAX(upper_cell_index(maxX), minCellX);
    s32 minCellZ = lower_cell_index(minZ);
    s32 maxCellZ = MAX(upper_cell_index(maxZ), minCellZ);

    for (cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
        for (cellX = minCellX; cellX <= maxCellX; cellX++) {
            add_surface_to_cell(dynamic, cellX, cellZ, surface);
        }
    }
}

/**
 * Initializes a Surface struct using the given vertex data
 * @param vertexData The raw data containing vertex positions
 * @param vertexIndices Helper which tells positions in vertexData to start reading vertices
 * @param dynamic If the surface belongs to an object or not
 */
static struct Surface *read_surface_data(TerrainData *vertexData, TerrainData **vertexIndices, u32 dynamic) {
    Vec3t v[3];
    Vec3t offset;
    s16 min, max;

    vec3_prod_val(offset, (*vertexIndices), 3);

    vec3s_copy(v[0], (vertexData + offset[0]));
    vec3s_copy(v[1], (vertexData + offset[1]));
    vec3s_copy(v[2], (vertexData + offset[2]));

    struct Surface *surface = alloc_surface(dynamic);

    vec3s_copy(surface->vertex1, v[0]);
    vec3s_copy(surface->vertex2, v[1]);
    vec3s_copy(surface->vertex3, v[2]);

    min_max_3s(v[0][1], v[1][1], v[2][1], &min, &max);
    surface->lowerY = (min - SURFACE_VERTICAL_BUFFER);
    surface->upperY = (max + SURFACE_VERTICAL_BUFFER);

    return surface;
}

s32 surf_has_no_cam_collision(s32 surfaceType) {
    switch (surfaceType) {
        case SURFACE_NO_CAM_COLLISION:
        case SURFACE_NO_CAM_COLLISION_77: // Unused
        case SURFACE_NO_CAM_COL_VERY_SLIPPERY:
        case SURFACE_SWITCH:
        case SURFACE_VANISH_CAP_WALLS:
        case SURFACE_ICE:
        case SURFACE_CRYSTAL:
        case SURFACE_HANGABLE_MESH:
            return TRUE;

    }
    return FALSE;
}

/**
 * Load in the surfaces for a given surface type. This includes setting the flags,
 * exertion, and room.
 */
static void load_static_surfaces(TerrainData **data, TerrainData *vertexData, s32 surfaceType) {
    s32 i;
    struct Surface *surface;

    s32 numSurfaces = *(*data)++;

    for (i = 0; i < numSurfaces; i++) {
        surface = read_surface_data(vertexData, data, FALSE);
        if (surface != NULL) {
            surface->type = surfaceType;

            add_surface(surface, FALSE);
        }

        *data += 3;
    }
}

/**
 * Read the data for vertices for reference by triangles.
 */
static TerrainData *read_vertex_data(TerrainData **data) {
    s32 numVertices = *(*data)++;

    TerrainData *vertexData = *data;
    *data += 3 * numVertices;

    return vertexData;
}

/**
 * Loads in special environmental regions, such as water, poison gas, and JRB fog.
 */
static void load_environmental_regions(TerrainData **data) {
    s32 i;

    gEnvironmentRegions = *data;
    s32 numRegions = *(*data)++;

    for (i = 0; i < numRegions; i++) {
        *data += 5;
        gEnvironmentLevels[i] = *(*data)++;
    }
}

/**
 * Allocate the dynamic surface pool for object collision.
 */
void alloc_surface_pools(void) {
    gMainSurfacePool = main_pool_alloc(MAIN_SURFACE_POOL_SIZE * sizeof(struct Surface), MEMORY_POOL_LEFT);
    gMainSurfaceNodePool = main_pool_alloc(MAIN_SURFACE_NODE_POOL_SIZE * sizeof(struct SurfaceNode), MEMORY_POOL_LEFT);
    gBlockSurfacePool = main_pool_alloc(BLOCK_SURFACE_POOL_SIZE * sizeof(struct Surface), MEMORY_POOL_LEFT);
    gBlockSurfaceNodePool = main_pool_alloc(BLOCK_SURFACE_NODE_POOL_SIZE * sizeof(struct SurfaceNode), MEMORY_POOL_LEFT);

    gSurfacePool = gMainSurfacePool;
    gSurfaceNodePool = gMainSurfaceNodePool;
    gSurfacesAllocated = &gMainSurfacesAllocated;
    gSurfaceNodesAllocated = &gMainSurfaceNodesAllocated;

    reset_red_coins_collected();
}

/**
 * Process the level file, loading in vertices, surfaces, some objects, and environmental
 * boxes (water, gas, JRB fog).
 */
void load_area_terrain(s32 index, TerrainData *data) {
    PUPPYPRINT_GET_SNAPSHOT();
    s32 terrainLoadType;
    TerrainData *vertexData = NULL;
    u32 surfacePoolData;

    // Initialize the data for this.
    gEnvironmentRegions = NULL;
    gMainSurfaceNodesAllocated = 0;
    gMainSurfacesAllocated = 0;

    clear_static_surfaces();

    // A while loop iterating through each section of the level data. Sections of data
    // are prefixed by a terrain "type." This type is reused for surfaces as the surface
    // type.
    while (TRUE) {
        terrainLoadType = *data++;

        if (TERRAIN_LOAD_IS_SURFACE_TYPE_LOW(terrainLoadType)) {
            load_static_surfaces(&data, vertexData, terrainLoadType);
        } else if (terrainLoadType == TERRAIN_LOAD_VERTICES) {
            vertexData = read_vertex_data(&data);
        // } else if (terrainLoadType == TERRAIN_LOAD_OBJECTS) {
        //     spawn_special_objects(index, &data);
        } else if (terrainLoadType == TERRAIN_LOAD_ENVIRONMENT) {
            load_environmental_regions(&data);
        } else if (terrainLoadType == TERRAIN_LOAD_CONTINUE) {
            continue;
        } else if (terrainLoadType == TERRAIN_LOAD_END) {
            break;
        } else if (TERRAIN_LOAD_IS_SURFACE_TYPE_HIGH(terrainLoadType)) {
            load_static_surfaces(&data, vertexData, terrainLoadType);
            continue;
        }
    }

    gNumStaticSurfaceNodes = gMainSurfaceNodesAllocated;
    gNumStaticSurfaces = gMainSurfacesAllocated;
    profiler_collision_update(first);
}

/**
 * If not in time stop, clear the surface partitions.
 */
void clear_dynamic_surfaces(void) {
    PUPPYPRINT_GET_SNAPSHOT();
    if (!(gTimeStopState & TIME_STOP_ACTIVE)) {
        //clear_dynamic_surface_references();

        gMainSurfacesAllocated = gNumStaticSurfaces;
        gMainSurfaceNodesAllocated = gNumStaticSurfaceNodes;
        clear_spatial_partition(&gDynamicSurfacePartition[0][0]);
    }
    profiler_collision_update(first);
}

void clear_block_surfaces(void) {
    gBlockSurfacesAllocated = 0;
    gBlockSurfaceNodesAllocated = 0;
}

/**
 * Applies an object's transformation to the object's vertices.
 */
void transform_object_vertices(TerrainData **data, TerrainData *vertexData) {
    Mat4 *objectTransform = &o->transform;

    register s32 numVertices = *(*data)++;

    register TerrainData *vertices = *data;

    if (o->header.gfx.throwMatrix == NULL) {
        o->header.gfx.throwMatrix = objectTransform;
        obj_build_transform_from_pos_and_angle(o, O_POS_INDEX, O_FACE_ANGLE_INDEX);
    }

    Mat4 transform;
    mtxf_scale_vec3f(transform, *objectTransform, o->header.gfx.scale);

    // Go through all vertices, rotating and translating them to transform the object.
    Vec3f pos;
    while (numVertices--) {
        vec3s_to_vec3f(pos, vertices);
        vertices += 3;

        //! No bounds check on vertex data
        linear_mtxf_mul_vec3_and_translate(transform, vertexData, pos);

        vertexData += 3;
    }

    *data = vertices;
}

/**
 * Load in the surfaces for the o. This includes setting the flags, exertion, and room.
 */
void load_object_surfaces(TerrainData **data, TerrainData *vertexData, u32 dynamic) {
    s32 i;

    s32 surfaceType = *(*data)++;
    s32 numSurfaces = *(*data)++;

    for (i = 0; i < numSurfaces; i++) {
        struct Surface *surface = read_surface_data(vertexData, data, dynamic);

        if (surface != NULL) {
            surface->object = o;
            surface->type = surfaceType;

            add_surface(surface, dynamic);
        }

        *data += 3;
    }
}

static TerrainData sVertexData[600];

/**
 * Transform an object's vertices, reload them, and render the object.
 */
extern u8 collision_load_total;
void load_object_collision_model(void) {
    if (collision_load_total >= 100) {
        return;
    }

    PUPPYPRINT_GET_SNAPSHOT();
    TerrainData *collisionData = o->collisionData;
    
    s32 inColRadius = cur_obj_should_load_collision();

    // Update if no Time Stop, in range, and in the current room.
    if (
        !(gTimeStopState & TIME_STOP_ACTIVE)
        && inColRadius
        && !(o->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)
    ) {
        collision_load_total++;
        if (cur_obj_has_behavior(bhvOnOffButton)) {
            collision_load_total += 2;
        }

        collisionData++;
        transform_object_vertices(&collisionData, sVertexData);

        // TERRAIN_LOAD_CONTINUE acts as an "end" to the terrain data.
        while (*collisionData != TERRAIN_LOAD_CONTINUE) {
            load_object_surfaces(&collisionData, sVertexData, TRUE);
        }
    }

    f32 marioDist = o->oDistanceToMario;

    // On an object's first frame, the distance is set to 19000.0f.
    // If the distance hasn't been updated, update it now.
    if (marioDist == 19000.0f) {
        marioDist = dist_between_objects(o, gMarioObject);
    }

    //COND_BIT((marioDist < o->oDrawingDistance), o->header.gfx.node.flags, GRAPH_RENDER_ACTIVE);
    profiler_collision_update(first);
}
