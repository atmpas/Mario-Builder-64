// metalstar

// 0x0302A6D8

// 0x0302A6F0
ALIGNED8 static const u8 metalstar_seg3_texture_0302A6F0[] = {
#include "actors/star/custom_metalstartex.rgba16.inc.c"
};

// 0x0302AEF0
ALIGNED8 static const u8 metalstar_seg3_texture_0302AEF0[] = {
#include "actors/star/star_eye.rgba16.inc.c"
};

// 0x0302B7B0 - 0x0302B870
const Gfx metalstar_seg3_sub_dl_body[] = {
    //gsSPLight(&star_seg3_lights.l, 1),
    //gsSPLight(&star_seg3_lights.a, 2),
    gsSPVertex(star_seg3_vertex_body, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  3,  0, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  7,  2,  4, 0x0),
    gsSP2Triangles( 8,  6,  4, 0x0,  9,  4, 10, 0x0),
    gsSP2Triangles( 9, 11,  4, 0x0,  4,  5, 10, 0x0),
    gsSP2Triangles(11,  8,  4, 0x0,  0,  2,  7, 0x0),
    gsSP2Triangles( 0,  7,  6, 0x0,  0,  6,  8, 0x0),
    gsSP2Triangles( 0,  8, 11, 0x0,  0, 11,  9, 0x0),
    gsSP2Triangles(10,  5,  0, 0x0, 10,  0,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0302B870 - 0x0302B908
const Gfx metalstar_seg3_dl_body[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(metalstar_seg3_texture_0302A6F0, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD), //! Dimensions loaded as 32x64 despite this texture having only 32x32 dimensions, harmless due to environment mapping (G_TEXTURE_GEN & gsSPTexture values)
    gsSPTexture(0x07C0, 0x07C0, 0, G_TX_RENDERTILE, G_ON),
    gsSPDisplayList(star_seg3_sub_dl_body),
    gsDPPipeSync(),
    gsSPTexture(0x07C0, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0302B908

// 0x0302B9C0 - 0x0302BA18
const Gfx metalstar_seg3_sub_dl_eyes[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, metalstar_seg3_texture_0302AEF0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    //gsSPLight(&star_seg3_lights.l, 1),
    //gsSPLight(&star_seg3_lights.a, 2),
    gsSPVertex(star_seg3_vertex_eyes, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0302BA18 - 0x0302BA88
const Gfx metalstar_seg3_dl_eyes[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(star_seg3_sub_dl_eyes),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
