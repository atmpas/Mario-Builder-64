#include <PR/ultratypes.h>

#include "sm64.h"
#include "actors/common1.h"
#include "gfx_dimensions.h"
#include "game_init.h"
#include "level_update.h"
#include "camera.h"
#include "print.h"
#include "ingame_menu.h"
#include "hud.h"
#include "segment2.h"
#include "area.h"
#include "save_file.h"
#include "print.h"
#include "engine/surface_load.h"
#include "engine/math_util.h"
#include "puppycam2.h"
#include "puppyprint.h"
#include "puppycamold.h"

#include "actors/group0.h"
#include "actors/group14.h"

#include "rovent.h"
#include "mb64.h"

#include "config.h"

/* @file hud.c
 * This file implements HUD rendering and power meter animations.
 * That includes stars, lives, coins, camera status, power meter, timer
 * cannon reticle, and the unused keys.
 **/

#ifdef BREATH_METER
#define HUD_BREATH_METER_X         40
#define HUD_BREATH_METER_Y         32
#define HUD_BREATH_METER_HIDDEN_Y -20
#endif

// ------------- FPS COUNTER ---------------
// To use it, call print_fps(x,y); every frame.
#define FRAMETIME_COUNT 30

OSTime frameTimes[FRAMETIME_COUNT];
u8 curFrameTimeIndex = 0;

#include "PR/os_convert.h"

#ifdef USE_PROFILER
float profiler_get_fps();
#else
// Call once per frame
f32 calculate_and_update_fps() {
    OSTime newTime = osGetTime();
    OSTime oldTime = frameTimes[curFrameTimeIndex];
    frameTimes[curFrameTimeIndex] = newTime;

    curFrameTimeIndex++;
    if (curFrameTimeIndex >= FRAMETIME_COUNT) {
        curFrameTimeIndex = 0;
    }
    return ((f32)FRAMETIME_COUNT * 1000000.0f) / (s32)OS_CYCLES_TO_USEC(newTime - oldTime);
}
#endif

void print_fps(s32 x, s32 y) {
#ifdef USE_PROFILER
    f32 fps = profiler_get_fps();
#else
    f32 fps = calculate_and_update_fps();
#endif
    char text[14];

    sprintf(text, "FPS %2.2f", fps);
#ifdef PUPPYPRINT
    print_small_text(x, y, text, PRINT_TEXT_ALIGN_LEFT, PRINT_ALL, FONT_OUTLINE);
#else
    print_text(x, y, text);
#endif
}

// ------------ END OF FPS COUNER -----------------

struct PowerMeterHUD {
    s8 animation;
    s16 x;
    s16 y;
};

struct CameraHUD {
    s16 status;
};

// Stores health segmented value defined by numHealthWedges
// When the HUD is rendered this value is 8, full health.
static s16 sPowerMeterStoredHealth;

static struct PowerMeterHUD sPowerMeterHUD = {
    POWER_METER_HIDDEN,
    HUD_POWER_METER_X,
    HUD_POWER_METER_HIDDEN_Y,
};

u8 MapOn;

void display_rocket_boot(void) {
    //bubble_seg4_dl_0401DD60
    Mtx *mtx;
    Mtx *smtx;
    
    mtx = alloc_display_list(sizeof(*mtx));

    if (mtx == NULL) {
        return;
    }
    
    //smtx = alloc_display_list(sizeof(*smtx));

    //if (smtx == NULL) {
    //    return;
    //}

    guTranslate(mtx, 30.0f, 75.0f, 0);
    //guScale(smtx, (f32)gMarioState->numAir*.002, (f32) gMarioState->numAir*.002, 0);

    gDPSetRenderMode(gDisplayListHead++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    //gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(smtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    gSPDisplayList(gDisplayListHead++, &rb_hud_Plane_mesh);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }

// void display_boss(void) {
//     //bubble_seg4_dl_0401DD60
//     Mtx *mtx;
//     Mtx *smtx;
    
//     mtx = alloc_display_list(sizeof(*mtx));

//     if (mtx == NULL) {
//         return;
//     }
    
//     //smtx = alloc_display_list(sizeof(*smtx));

//     //if (smtx == NULL) {
//     //    return;
//     //}

//     guTranslate(mtx, 30.0f, 75.0f, 0);
//     //guScale(smtx, (f32)gMarioState->numAir*.002, (f32) gMarioState->numAir*.002, 0);

//     gDPSetRenderMode(gDisplayListHead++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
//     gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
//     //gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(smtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

//     gSPDisplayList(gDisplayListHead++, &bs_hud_Plane_mesh);
//     gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
//     }

void display_rocket_boot_bar(void) {
    //bubble_seg4_dl_0401DD60
    Mtx *mtx;
    Mtx *smtx;
    
    mtx = alloc_display_list(sizeof(*mtx));

    if (mtx == NULL) {
        return;
    }
    
    smtx = alloc_display_list(sizeof(*smtx));

    if (smtx == NULL) {
        return;
    }

    guTranslate(mtx, 30.0f, 75.0f, 0);
    guScale(smtx, (f32)1.0, (f32) gMarioState->RFuel*.01, 0);

    gDPSetRenderMode(gDisplayListHead++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(smtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    gSPDisplayList(gDisplayListHead++, &rb_bar_Plane_001_mesh);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }

#include "src/engine/math_util.h"

f32 bar_visual;
#define BAR_TARGET (f32)(gMarioState->BossHealth)/(f32)(gMarioState->BossHealthMax)

// void display_boss_bar(void) {
//     //bubble_seg4_dl_0401DD60
//     Mtx *mtx;
//     Mtx *smtx;
    
//     mtx = alloc_display_list(sizeof(*mtx));

//     if (mtx == NULL) {
//         return;
//     }
    
//     smtx = alloc_display_list(sizeof(*smtx));

//     if (smtx == NULL) {
//         return;
//     }

//     guTranslate(mtx, 30.0f, 75.0f, 0);

//     bar_visual = lerp(bar_visual, BAR_TARGET, 0.1f);

//     guScale(smtx, (f32)1.0, bar_visual, 0);

//     gDPSetRenderMode(gDisplayListHead++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
//     gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
//     gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(smtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

//     gSPDisplayList(gDisplayListHead++, &bs_bar_Plane_001_mesh);
//     gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
//     }

void display_air(void) {
    //bubble_seg4_dl_0401DD60
    Mtx *mtx;
    Mtx *smtx;

    f32 xpos = (gMarioState->RFuel > 0 ? 70.f : 35.f);
    
    mtx = alloc_display_list(sizeof(*mtx));

    if (mtx == NULL) {
        return;
    }
    
    smtx = alloc_display_list(sizeof(*smtx));

    if (smtx == NULL) {
        return;
    }

    guTranslate(mtx, xpos, 40.0f, 0);
    guScale(smtx, (f32)gMarioState->numAir*.0014, (f32) gMarioState->numAir*.0014, 0);

    gDPSetRenderMode(gDisplayListHead++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(smtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    gSPDisplayList(gDisplayListHead++, &bubble_seg4_dl_0401DD60);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }

//title_Plane_001_mesh_vtx_0

void display_title(void) {
    //bubble_seg4_dl_0401DD60
    Mtx *mtx;
    Mtx *smtx;
    
    mtx = alloc_display_list(sizeof(*mtx));

    if (mtx == NULL) {
        return;
    }

    guTranslate(mtx, 160.0f, 120.0f, 0);

    gDPSetRenderMode(gDisplayListHead++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

    //gSPDisplayList(gDisplayListHead++, &title_Plane_001_mesh);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }

// void display_minimap(void) {
//     Mtx *mtx;
//     f32 mx;
//     f32 my;
//     u8 wideoffet9 = 0;

//     if (0) {
//         wideoffet9 = 40;
//         }

//     mtx = alloc_display_list(sizeof(*mtx));

//     if (mtx == NULL) {
//         return;
//     }

//     mx = ((gMarioState->pos[0]/14834)*64)-wideoffet9;
//     my = (gMarioState->pos[2]/14834)*64;

//     guTranslate(mtx, (f32) 268-mx, (f32) 52+my, 0);
//     gDPSetRenderMode(gDisplayListHead++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
//     gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
//     MapOn = TRUE;

//     gDPSetScissor(gDisplayListHead++,G_SC_NON_INTERLACE, 236+(wideoffet9/4), 156, 300, 220 );
//     switch(gCurrLevelNum) {
//         case LEVEL_BOB:
//             gSPDisplayList(gDisplayListHead++, &Map1_Plane_mesh);
//             break;
//         case LEVEL_WF:
//             gSPDisplayList(gDisplayListHead++, &Map2_Plane_mesh);
//             break;
//         case LEVEL_JRB:
//             gSPDisplayList(gDisplayListHead++, &Map3_Plane_mesh);
//             break;
//         case LEVEL_CCM:
//             gSPDisplayList(gDisplayListHead++, &Map4_Plane_mesh);
//             break;
//         case LEVEL_BBH:
//             gSPDisplayList(gDisplayListHead++, &Map5_Plane_mesh);
//             break;
//         case LEVEL_HMC:
//             gSPDisplayList(gDisplayListHead++, &Map6_Plane_mesh);
//             break;
//         case LEVEL_LLL:
//             gSPDisplayList(gDisplayListHead++, &Map7_Plane_mesh);
//             break;
//         case LEVEL_DDD:
//             gSPDisplayList(gDisplayListHead++, &Map9_Plane_mesh);
//             break;
//         case LEVEL_SL:
//             gSPDisplayList(gDisplayListHead++, &Map10_Plane_mesh);
//             break;
//         case LEVEL_WDW:
//             gSPDisplayList(gDisplayListHead++, &Map11_Plane_mesh);
//             break; 
//         case LEVEL_TTM:
//             gSPDisplayList(gDisplayListHead++, &Map12_Plane_mesh);
//             break;
//         case LEVEL_THI:
//             gSPDisplayList(gDisplayListHead++, &Map13_Plane_mesh);
//             break;
//         case LEVEL_TTC:
//             gSPDisplayList(gDisplayListHead++, &Map14_Plane_mesh);
//             break;

//         default:
//             MapOn = FALSE;
//         }
//     gSPPopMatrix(gDisplayListHead++, 0);
// }

// void display_arrow(void) {
//     Mtx *mtx;
//     Mtx *rmtx;
//     u8 wideoffet11 = 0;

//     if (0) {
//         wideoffet11 = 40;
//         }

//     mtx = alloc_display_list(sizeof(*mtx));

//     if (mtx == NULL) {
//         return;
//     }

//    rmtx = alloc_display_list(sizeof(*rmtx));

//     if (rmtx == NULL) {
//         return;
//     }

//     guTranslate(mtx, (f32) 268+wideoffet11, (f32) 52, 0);
//     gDPSetRenderMode(gDisplayListHead++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);

//     gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
//     guRotate(rmtx, gMarioState->faceAngle[1] / 180.0f + 180.0f, 0.0f, 0.0f, 1.0f);
//     gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(rmtx++),G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
//     gSPDisplayList(gDisplayListHead++, &arrow_Plane_001_mesh);
//     gSPPopMatrix(gDisplayListHead++, 0);
// }

// Power Meter timer that keeps counting when it's visible.
// Gets reset when the health is filled and stops counting
// when the power meter is hidden.
s32 sPowerMeterVisibleTimer = 0;

#ifdef BREATH_METER
static s16 sBreathMeterStoredValue;
static struct PowerMeterHUD sBreathMeterHUD = {
    BREATH_METER_HIDDEN,
    HUD_BREATH_METER_X,
    HUD_BREATH_METER_HIDDEN_Y,
};
s32 sBreathMeterVisibleTimer = 0;
#endif

static struct CameraHUD sCameraHUD = { CAM_STATUS_NONE };

/**
 * Renders a rgba16 16x16 glyph texture from a table list.
 */
void render_hud_tex_lut(s32 x, s32 y, Texture *texture) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    gSPDisplayList(gDisplayListHead++, &dl_hud_img_load_tex_block);
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 15) << 2, (y + 15) << 2,
                        G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);
}

/**
 * Renders a rgba16 8x8 glyph texture from a table list.
 */
void render_hud_small_tex_lut(s32 x, s32 y, Texture *texture) {
    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0,
                G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD);
    gDPTileSync(gDisplayListHead++);
    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 2, 0, G_TX_RENDERTILE, 0,
                G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD);
    gDPSetTileSize(gDisplayListHead++, G_TX_RENDERTILE, 0, 0, (8 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC);
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_16b_BYTES));
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 7) << 2, (y + 7) << 2, G_TX_RENDERTILE,
                        0, 0, 4 << 10, 1 << 10);
}

/**
 * Renders power meter health segment texture using a table list.
 */
void render_power_meter_health_segment(s16 numHealthWedges) {
    Texture *(*healthLUT)[] = segmented_to_virtual(&power_meter_health_segments_lut);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1,
                       (*healthLUT)[numHealthWedges - 1]);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES));
    gSP1Triangle(gDisplayListHead++, 0, 1, 2, 0);
    gSP1Triangle(gDisplayListHead++, 0, 2, 3, 0);
}

/**
 * Renders power meter display lists.
 * That includes the "POWER" base and the colored health segment textures.
 */
void render_dl_power_meter(s16 numHealthWedges) {
    Mtx *mtx = alloc_display_list(sizeof(Mtx));

    if (mtx == NULL) {
        return;
    }

    f32 power_meter_x_offset = 0.0f;
    if (mb64_sram_configuration.option_flags & (1<<OPT_HUDLAYOUT)) {
        power_meter_x_offset = 148.0f;
    }

    guTranslate(mtx, (f32) sPowerMeterHUD.x + power_meter_x_offset, (f32) sPowerMeterHUD.y, 0);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, &dl_power_meter_base);

    if (numHealthWedges != 0) {
        gSPDisplayList(gDisplayListHead++, &dl_power_meter_health_segments_begin);
        render_power_meter_health_segment(numHealthWedges);
        gSPDisplayList(gDisplayListHead++, &dl_power_meter_health_segments_end);
    }

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/**
 * Power meter animation called when there's less than 8 health segments
 * Checks its timer to later change into deemphasizing mode.
 */
void animate_power_meter_emphasized(void) {
    s16 hudDisplayFlags = gHudDisplay.flags;

    if (!(hudDisplayFlags & HUD_DISPLAY_FLAG_EMPHASIZE_POWER)) {
        if (sPowerMeterVisibleTimer == 45.0f) {
            sPowerMeterHUD.animation = POWER_METER_DEEMPHASIZING;
        }
    } else {
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Power meter animation called after emphasized mode.
 * Moves power meter y pos speed until it's at 200 to be visible.
 */
static void animate_power_meter_deemphasizing(void) {
    s16 speed = 5;

    if (sPowerMeterHUD.y > HUD_POWER_METER_Y - 20) speed = 3;
    if (sPowerMeterHUD.y > HUD_POWER_METER_Y - 10) speed = 2;
    if (sPowerMeterHUD.y > HUD_POWER_METER_Y -  5) speed = 1;

    sPowerMeterHUD.y += speed;

    if (sPowerMeterHUD.y > HUD_POWER_METER_Y) {
        sPowerMeterHUD.y = HUD_POWER_METER_Y;
        sPowerMeterHUD.animation = POWER_METER_VISIBLE;
    }
}

/**
 * Power meter animation called when there's 8 health segments.
 * Moves power meter y pos quickly until it's at 301 to be hidden.
 */
static void animate_power_meter_hiding(void) {
    sPowerMeterHUD.y += 20;
    if (sPowerMeterHUD.y > HUD_POWER_METER_HIDDEN_Y) {
        sPowerMeterHUD.animation = POWER_METER_HIDDEN;
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Handles power meter actions depending of the health segments values.
 */
void handle_power_meter_actions(s16 numHealthWedges) {
    // Show power meter if health is not full, less than 8
    if (numHealthWedges < 8 && sPowerMeterStoredHealth == 8
        && sPowerMeterHUD.animation == POWER_METER_HIDDEN) {
        sPowerMeterHUD.animation = POWER_METER_EMPHASIZED;
        sPowerMeterHUD.y = HUD_POWER_METER_EMPHASIZED_Y;
    }

    // Show power meter if health is full, has 8
    if (numHealthWedges == 8 && sPowerMeterStoredHealth == 7) {
        sPowerMeterVisibleTimer = 0;
    }

    // After health is full, hide power meter
    if (numHealthWedges == 8 && sPowerMeterVisibleTimer > 45.0f) {
        sPowerMeterHUD.animation = POWER_METER_HIDING;
    }

    // Update to match health value
    sPowerMeterStoredHealth = numHealthWedges;

#ifndef BREATH_METER
    // If Mario is swimming, keep power meter visible
    if (gPlayerCameraState->action & ACT_FLAG_SWIMMING) {
        if (sPowerMeterHUD.animation == POWER_METER_HIDDEN
            || sPowerMeterHUD.animation == POWER_METER_EMPHASIZED) {
            sPowerMeterHUD.animation = POWER_METER_DEEMPHASIZING;
            sPowerMeterHUD.y = HUD_POWER_METER_EMPHASIZED_Y;
        }
        sPowerMeterVisibleTimer = 0;
    }
#endif
}

/**
 * Renders the power meter that shows when Mario is in underwater
 * or has taken damage and has less than 8 health segments.
 * And calls a power meter animation function depending of the value defined.
 */
//

void render_hud_power_meter(void) {
    s16 loop;
    s16 loop2;
    u8 wideoffet5 = 0;

    if (0) {
        wideoffet5 = 22;
        }
    //RENDER BLANK METERS FIRST
    loop = gMarioState->numMaxHP;
    loop2 = gMarioState->health > 0 ? gMarioState->health >> 8 : 0;
    while (loop > 0) {
        loop --;
        if (loop >= loop2) {
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)-wideoffet5+(loop*8), 30, "V");
            }
            else
            {
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)-wideoffet5+(loop*8), 30, "Z");
            }
        }

    loop = gMarioState->numMaxFP;
    loop2 = gMarioState->numBadgePoints;
    while (loop > 0) {
        loop --;
        if (loop >= loop2) {
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)-wideoffet5+(loop*8), 20, "V");
            }
            else
            {
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)-wideoffet5+(loop*8), 20, "X");
            }
        }

    //render_dl_power_meter(shownHealthWedges);

    //RENDER COINS
}

void render_hud_power_meter_vanilla(void) {
    s16 shownHealthWedges = gHudDisplay.wedges;
    if (sPowerMeterHUD.animation != POWER_METER_HIDING) handle_power_meter_actions(shownHealthWedges);
    if (sPowerMeterHUD.animation == POWER_METER_HIDDEN) return;
    switch (sPowerMeterHUD.animation) {
        case POWER_METER_EMPHASIZED:    animate_power_meter_emphasized();    break;
        case POWER_METER_DEEMPHASIZING: animate_power_meter_deemphasizing(); break;
        case POWER_METER_HIDING:        animate_power_meter_hiding();        break;
        default:                                                             break;
    }
    render_dl_power_meter(shownHealthWedges);
    sPowerMeterVisibleTimer++;
}

#ifdef BREATH_METER
/**
 * Renders breath meter health segment texture using a table list.
 */
void render_breath_meter_segment(s16 numBreathWedges) {
    Texture *(*breathLUT)[];
    breathLUT = segmented_to_virtual(&breath_meter_segments_lut);
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, (*breathLUT)[numBreathWedges - 1]);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES));
    gSP1Triangle(gDisplayListHead++, 0, 1, 2, 0);
    gSP1Triangle(gDisplayListHead++, 0, 2, 3, 0);
}

/**
 * Renders breath meter display lists.
 * That includes the base and the colored segment textures.
 */
void render_dl_breath_meter(s16 numBreathWedges) {
    Mtx *mtx = alloc_display_list(sizeof(Mtx));

    if (mtx == NULL) {
        return;
    }

    guTranslate(mtx, (f32) sBreathMeterHUD.x, (f32) sBreathMeterHUD.y, 0);
    gSPMatrix(      gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),
                    G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList( gDisplayListHead++, &dl_breath_meter_base);
    if (numBreathWedges != 0) {
        gSPDisplayList(gDisplayListHead++, &dl_breath_meter_health_segments_begin);
        render_breath_meter_segment(numBreathWedges);
        gSPDisplayList(gDisplayListHead++, &dl_breath_meter_health_segments_end);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/**
 * Breath meter animation called after emphasized mode.
 * Moves breath meter y pos speed until it's visible.
 */
static void animate_breath_meter_sliding_in(void) {
    approach_s16_symmetric_bool(&sBreathMeterHUD.y, HUD_BREATH_METER_Y, 5);
    if (sBreathMeterHUD.y         == HUD_BREATH_METER_Y) {
        sBreathMeterHUD.animation = BREATH_METER_VISIBLE;
    }
}

/**
 * Breath meter animation called when there's 8 health segments.
 * Moves breath meter y pos quickly until it's hidden.
 */
static void animate_breath_meter_sliding_out(void) {
    approach_s16_symmetric_bool(&sBreathMeterHUD.y, HUD_BREATH_METER_HIDDEN_Y, 20);
    if (sBreathMeterHUD.y         == HUD_BREATH_METER_HIDDEN_Y) {
        sBreathMeterHUD.animation = BREATH_METER_HIDDEN;
    }
}

/**
 * Handles breath meter actions depending of the health segments values.
 */
void handle_breath_meter_actions(s16 numBreathWedges) {
    // Show breath meter if health is not full, less than 8
    if ((numBreathWedges < 8) && (sBreathMeterStoredValue == 8) && sBreathMeterHUD.animation == BREATH_METER_HIDDEN) {
        sBreathMeterHUD.animation = BREATH_METER_SHOWING;
        // sBreathMeterHUD.y         = HUD_BREATH_METER_Y;
    }
    // Show breath meter if breath is full, has 8
    if ((numBreathWedges == 8) && (sBreathMeterStoredValue  == 7)) sBreathMeterVisibleTimer  = 0;
    // After breath is full, hide breath meter
    if ((numBreathWedges == 8) && (sBreathMeterVisibleTimer > 45)) sBreathMeterHUD.animation = BREATH_METER_HIDING;
    // Update to match breath value
    sBreathMeterStoredValue = numBreathWedges;
    // If Mario is swimming, keep breath meter visible
    if (gPlayerCameraState->action & ACT_FLAG_SWIMMING) {
        if (sBreathMeterHUD.animation == BREATH_METER_HIDDEN) {
            sBreathMeterHUD.animation = BREATH_METER_SHOWING;
        }
        sBreathMeterVisibleTimer = 0;
    }
}

void render_hud_breath_meter(void) {
    s16 shownBreathAmount = gHudDisplay.breath;
    if (sBreathMeterHUD.animation != BREATH_METER_HIDING) handle_breath_meter_actions(shownBreathAmount);
    if (sBreathMeterHUD.animation == BREATH_METER_HIDDEN) return;
    switch (sBreathMeterHUD.animation) {
        case BREATH_METER_SHOWING:       animate_breath_meter_sliding_in();  break;
        case BREATH_METER_HIDING:        animate_breath_meter_sliding_out(); break;
        default:                                                             break;
    }
    render_dl_breath_meter(shownBreathAmount);
    sBreathMeterVisibleTimer++;
}
#endif


/**
 * Renders the amount of lives Mario has.
 */
void render_hud_mario_lives(void) {
    s8 showX = 0;
    u8 wideoffet2 = 0;

    if (gMarioState->gCurrMinigame == 0) {
        print_text_fmt_int2(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, ",%d", gMarioState->numGlobalCoins, gMarioState->numMaxGlobalCoins);
        }
}

#ifdef VANILLA_STYLE_CUSTOM_DEBUG
void render_debug_mode(void) {
    print_text(180, 40, "DEBUG MODE");
    print_text_fmt_int(5, 20, "Z %d", gMarioState->pos[2]);
    print_text_fmt_int(5, 40, "Y %d", gMarioState->pos[1]);
    print_text_fmt_int(5, 60, "X %d", gMarioState->pos[0]);
    print_text_fmt_int(10, 100, "SPD %d", (s32) gMarioState->forwardVel);
    print_text_fmt_int(10, 120, "ANG 0*%04x", (u16) gMarioState->faceAngle[1]);
    print_fps(10,80);
}
#endif

/**
 * Renders the amount of coins collected.
 */
void render_hud_coins(void) {
    if (mb64_sram_configuration.option_flags & (1<<OPT_HUDLAYOUT)) {
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-21, "$"); // 'Coin' glyph
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)+16, HUD_TOP_Y-21, "*"); // 'X' glyph
        print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)+32, HUD_TOP_Y-21, "%d", gHudDisplay.coins);

        if (gRedCoinsCollected > 0) {
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-42, "@"); // 'Coin' glyph
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)+16, HUD_TOP_Y-42, "*"); // 'X' glyph
            print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)+32, HUD_TOP_Y-42, "%d", gRedCoinsCollected);
        }
    } else {
        print_text(168, HUD_TOP_Y, "$"); // 'Coin' glyph
        if (gHudDisplay.coins < 1000) {
            print_text(184, HUD_TOP_Y, "*"); // 'X' glyph
            print_text_fmt_int(198, HUD_TOP_Y, "%d", gHudDisplay.coins);
        } else {
            print_text_fmt_int(184, HUD_TOP_Y, "%d", gHudDisplay.coins);
        }

        if (gRedCoinsCollected > 0) {
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "@"); // 'Coin' glyph
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)+16, HUD_TOP_Y, "*"); // 'X' glyph
            print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)+16+14, HUD_TOP_Y, "%d", gRedCoinsCollected);
        }
    }
}

/**
 * Renders the amount of stars collected.
 * Disables "X" glyph when Mario has 100 stars or more.
 */
 s8 minuz[] = {45,0};
void render_hud_stars(void) {
    u8 current_stars = mb64_play_stars;
    u8 max_stars = mb64_play_stars_max;

    if (gHudFlash == HUD_FLASH_STARS && gGlobalTimer & 0x8) return;
    s8 showX = 1;//(gHudDisplay.stars < 100);

    if (mb64_sram_configuration.option_flags & (1<<OPT_HUDLAYOUT)) {
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "^"); // 'Star' glyph
        print_text((GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22) + 16), HUD_TOP_Y, "*"); // 'X' glyph
        print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22) + 32, HUD_TOP_Y, "%d", current_stars);
    } else {
        print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X), HUD_TOP_Y, "^"); // 'Star' glyph
        if (showX) print_text((GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X) + 16), HUD_TOP_Y, "*"); // 'X' glyph
        print_text_fmt_int((showX * 14) + GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X - 16), HUD_TOP_Y, "%d", current_stars);
    }
}

/**
 * Unused function that renders the amount of keys collected.
 * Leftover function from the beta version of the game.
 */
void render_hud_keys(void) {
    s16 i;

    for (i = 0; i < gHudDisplay.keys; i++) {
        print_text((i * 16) + 220, 142, "|"); // unused glyph - beta key
    }
}

/**
 * Renders the timer when Mario start sliding in PSS.
 */
 #define HUD_TIMER_MODERN_OFFSET -100
void render_hud_timer(void) {
    Texture *(*hudLUT)[58] = segmented_to_virtual(&main_hud_lut);
    u32 timerValFrames = gHudDisplay.timer;
    if (mb64_sram_configuration.option_flags & (1<<OPT_SPEEDRUNTIMER)) {
        timerValFrames = mb64_play_speedrun_timer;
    }

    u16 timerMins = timerValFrames / (30 * 60);
    u16 timerSecs = (timerValFrames - (timerMins * 1800)) / 30;
    u16 timerFracSecs = ((timerValFrames - (timerMins * 1800) - (timerSecs * 30)) & 0xFFFF) / 3;

    s8 minxoffset = 0;
    if (timerMins > 9) {
        minxoffset = -12;
    }
/*
#if MULTILANG
    switch (eu_get_language()) {
        case LANGUAGE_ENGLISH: print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 185,  "TIME"); break;
        case LANGUAGE_FRENCH:  print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(155), 185, "TEMPS"); break;
        case LANGUAGE_GERMAN:  print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 185,  "ZEIT"); break;
    }
#else
*/

    if (mb64_sram_configuration.option_flags & (1<<OPT_HUDLAYOUT)) {
        //modern
        print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(91)+HUD_TIMER_MODERN_OFFSET+minxoffset, HUD_TOP_Y, "%0d", timerMins);
        print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(71)+HUD_TIMER_MODERN_OFFSET, HUD_TOP_Y, "%02d", timerSecs);
        print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(37)+HUD_TIMER_MODERN_OFFSET, HUD_TOP_Y, "%d", timerFracSecs);

        gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
        render_hud_tex_lut(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(81)+HUD_TIMER_MODERN_OFFSET, 8, (*hudLUT)[GLYPH_APOSTROPHE]);
        render_hud_tex_lut(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(46)+HUD_TIMER_MODERN_OFFSET, 8, (*hudLUT)[GLYPH_DOUBLE_QUOTE]);
        gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
    } else {
        print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150)+minxoffset, 185, "TIME");

        print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(91)+minxoffset, 185, "%0d", timerMins);
        print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(71), 185, "%02d", timerSecs);
        print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(37), 185, "%d", timerFracSecs);

        gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
        render_hud_tex_lut(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(81), 32, (*hudLUT)[GLYPH_APOSTROPHE]);
        render_hud_tex_lut(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(46), 32, (*hudLUT)[GLYPH_DOUBLE_QUOTE]);
        gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
    }
}

/**
 * Sets HUD status camera value depending of the actions
 * defined in update_camera_status.
 */
void set_hud_camera_status(s16 status) {
    sCameraHUD.status = status;
}

/**
 * Renders camera HUD glyphs using a table list, depending of
 * the camera status called, a defined glyph is rendered.
 */
void render_hud_camera_status(void) {
    Texture *(*cameraLUT)[6] = segmented_to_virtual(&main_hud_camera_lut);
    s32 x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_CAMERA_X);
    s32 y = 205;

    if (sCameraHUD.status == CAM_STATUS_NONE) {
        return;
    }

    gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
    render_hud_tex_lut(x, y, (*cameraLUT)[GLYPH_CAM_CAMERA]);

    switch (sCameraHUD.status & CAM_STATUS_MODE_GROUP) {
        case CAM_STATUS_MARIO:
            render_hud_tex_lut(x + 16, y, (*cameraLUT)[GLYPH_CAM_MARIO_HEAD]);
            break;
        case CAM_STATUS_LAKITU:
            render_hud_tex_lut(x + 16, y, (*cameraLUT)[GLYPH_CAM_LAKITU_HEAD]);
            break;
        case CAM_STATUS_FIXED:
            render_hud_tex_lut(x + 16, y, (*cameraLUT)[GLYPH_CAM_FIXED]);
            break;
    }

    switch (sCameraHUD.status & CAM_STATUS_C_MODE_GROUP) {
        case CAM_STATUS_C_DOWN:
            render_hud_small_tex_lut(x + 4, y + 16, (*cameraLUT)[GLYPH_CAM_ARROW_DOWN]);
            break;
        case CAM_STATUS_C_UP:
            render_hud_small_tex_lut(x + 4, y - 8, (*cameraLUT)[GLYPH_CAM_ARROW_UP]);
            break;
    }

    gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
}






u8 lframes = 0;
u8 lease = 2;
/**
 * Render HUD strings using hudDisplayFlags with it's render functions,
 * excluding the cannon reticle which detects a camera preset for it.
 */
void render_hud(void) {
    u8 wideoffet4 = 0;
    //gDPPipelineMode(gDisplayListHead++,G_PM_1PRIMITIVE);
    s16 hudDisplayFlags = gHudDisplay.flags;

    if (0) {
        wideoffet4 = 22;
    }

    if (hudDisplayFlags == HUD_DISPLAY_NONE) {
        sPowerMeterHUD.animation = POWER_METER_HIDDEN;
        sPowerMeterStoredHealth = 8;
        sPowerMeterVisibleTimer = 0;
#ifdef BREATH_METER
        sBreathMeterHUD.animation = BREATH_METER_HIDDEN;
        sBreathMeterStoredValue = 8;
        sBreathMeterVisibleTimer = 0;
#endif
    } else {
#ifdef VERSION_EU
        // basically create_dl_ortho_matrix but guOrtho screen width is different
        Mtx *mtx = alloc_display_list(sizeof(*mtx));

        if (mtx == NULL) {
            return;
        }

        create_dl_identity_matrix();
        guOrtho(mtx, -16.0f, SCREEN_WIDTH + 16, 0, SCREEN_HEIGHT, -10.0f, 10.0f, 1.0f);
        gSPPerspNormalize(gDisplayListHead++, 0xFFFF);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx),
                  G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
#else
        create_dl_ortho_matrix();
#endif

        // if (gCurrentArea != NULL && gCurrentArea->camera->mode == CAMERA_MODE_INSIDE_CANNON) {
        //     render_hud_cannon_reticle();
        // }

    if (gCurrDemoInput != NULL) {
        display_title();
        }

        gMarioState->toggleHud = (mb64_sram_configuration.option_flags & (1<<OPT_HUD));

        if ((mb64_mode == MB64_MODE_PLAY) && (revent_hud) && (gMarioState->toggleHud)&&(gCurrDemoInput == NULL)&&(gMenuMode == -1)&&(gDialogID != 1)) {

#ifndef DISABLE_LIVES
            if (hudDisplayFlags & HUD_DISPLAY_FLAG_LIVES) {
                //render_hud_mario_lives();
            }
#endif

            if (hudDisplayFlags & HUD_DISPLAY_FLAG_COIN_COUNT) {
                render_hud_coins();
            }

            // //i should have programmed a proper quest system lmfao
            // if (gMarioState->DeadRexMissionActivate) {
            //     print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)-wideoffet4, HUD_TOP_Y-18, "%02dQ10", gMarioState->DeadRexes);
            //     }
            // if (gMarioState->DeadCowboyMissionActivate) {
            //     print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)-wideoffet4, HUD_TOP_Y-18, "%dQ7", gMarioState->DeadRexes);
            //     }
            // if (gMarioState->CheeseMissionActivate) {
            //     //this hardcoded bullshit is so fucking awful. if i made cursed mirror again i would make a quest SYSTEM! AUTOMATION! WE LIVE
            //     //IN THE AGE OF FUCKING AUTOMATION. AND MY BITCHASS HARD CODED THESE MISSIONS. stupid bitch, fycj you
            //     if (gCurrLevelNum == LEVEL_TTC) {
            //         //highcane mission
            //         print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)-wideoffet4, HUD_TOP_Y-18, "%dQ20", gMarioState->CheeseCollection);
            //     } else {
            //         print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)-wideoffet4, HUD_TOP_Y-18, "%dQ5", gMarioState->CheeseCollection);
            //     }
            // }
            // if (gMarioState->SockMissionActivate) {
            //     print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22)-wideoffet4, HUD_TOP_Y-18, "%dQ7", gMarioState->SockCollection);
            //     }

            // switch(gMarioState->gCurrMinigame) {
            //     case 1://arena
            //         print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "ROUND %d", gMarioState->EA_WAVES);
            //         print_text_fmt_int2(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-18, "%dQ%d", gMarioState->EA_LEFT, gMarioState->EA_TOTAL);
            //         if (minigame_real) {
            //             print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-36, "HI %d", save_file_get_hiscore(0));
            //         }
            //     break;
            //     case 2://hotrope
            //         print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "SCORE %d", gMarioState->EA_WAVES);
            //         if (minigame_real) {
            //             print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-18, "HI %d", save_file_get_hiscore(1));
            //         }
            //     break;
            //     case 3://hexagonheat
            //         print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "SCORE %d", gMarioState->EA_WAVES);
            //         if (minigame_real) {
            //             print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-18, "HI %d", save_file_get_hiscore(2));
            //         }
            //     break;
            //     case 4://snakio
            //         print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "LENGTH %d", gMarioState->EA_WAVES);
            //         if (minigame_real) {
            //             print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-18, "HI %d", save_file_get_hiscore(3));
            //         }
            //     break;
            //     case 5://edsurv
            //         print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "EDWARDS %d", gMarioState->EA_WAVES);
            //         if (minigame_real) {
            //             print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-18, "HI %d", save_file_get_hiscore(4));
            //         }
            //     break;
            //     case 6://bapple
            //         print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "SCORE %d", gMarioState->EA_WAVES);
            //         if (minigame_real) {
            //             print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-18, "HI %d", save_file_get_hiscore(5));
            //         }
            //     break;
            //     case 7://flappy bird
            //         print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y, "SCORE %d", gMarioState->EA_WAVES);
            //         if (minigame_real) {
            //             print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), HUD_TOP_Y-18, "HI %d", save_file_get_hiscore(6));
            //         }
            //     break;
            // }

            //Use this later for the star radar badge
            if (gMarioState->StarRadarExist == TRUE) {
                gMarioState->StarRadarExist = FALSE;
                print_text_fmt_int(gMarioState->ScreenPosX,gMarioState->ScreenPosY,"^",0);

                //print_text_fmt_int(40,40,"%d",gMarioState->ScreenPosX);
                //print_text_fmt_int(40,80,"Y %d",gMarioState->ScreenPosY);
            }

            if (hudDisplayFlags & HUD_DISPLAY_FLAG_STAR_COUNT) {
                render_hud_stars();
            }

            if (hudDisplayFlags & HUD_DISPLAY_FLAG_KEYS) {
                render_hud_keys();
            }

            if (gMarioState->numAir < 700) {
                display_air();
            }
            if (gMarioState->RFuel > 0) {
                display_rocket_boot();
                display_rocket_boot_bar();
            }

#ifdef BREATH_METER
            if (hudDisplayFlags & HUD_DISPLAY_FLAG_BREATH_METER) render_hud_breath_meter();
#endif

            if (hudDisplayFlags & HUD_DISPLAY_FLAG_CAMERA_AND_POWER) {
                if (mb64_lopt_game == MB64_GAME_VANILLA) {
                    render_hud_power_meter_vanilla();
                } else {
                    render_hud_power_meter();
                }
                render_hud_camera_status();
            }

            if ((hudDisplayFlags & HUD_DISPLAY_FLAG_TIMER)||(mb64_sram_configuration.option_flags & (1<<OPT_SPEEDRUNTIMER))) {
                render_hud_timer();
            }

            /*
            if (gMarioState->NewTimer > 0) {
                if (!(gMarioState->Options & (1<<OPT_MINIMAP))) {
                    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X),20, "j %d", gMarioState->NewTimer);
                    }else{
                    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(HUD_STARS_X),90, "j %d", gMarioState->NewTimer);
                    }
                }
            */

            /*
            if (gMarioState->Options & (1<<OPT_MINIMAP)) {
                if (gCurrentArea->index == 1) {
                    display_minimap();

                    if (MapOn) {
                        display_arrow();
                    }
                }
            }
            */
        }

#ifdef VANILLA_STYLE_CUSTOM_DEBUG
        if (gCustomDebugMode) {
            render_debug_mode();
        }
#endif
    }
}
