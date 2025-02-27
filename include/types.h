#ifndef TYPES_H
#define TYPES_H

// This file contains various data types used in Super Mario 64 that don't yet
// have an appropriate header.

#include <ultra64.h>
#include "macros.h"
#include "config.h"

#define BIT(i)  (1 << (i))
#define BITMASK(size) ((BIT(size)) - 1)
#define SHIFTED_BITMASK(size, shift) (BITMASK(size) << shift)

// #define COND_BIT(cond, dst, flag) { (dst) &= ~(flag); if (cond) (dst) |= (flag); }
#define COND_BIT(cond, dst, flag) { \
    if ((cond)) {                   \
        (dst) |=  (flag);           \
    } else {                        \
        (dst) &= ~(flag);           \
    }                               \
}

#define SCREEN_CENTER_X (SCREEN_WIDTH  / 2)
#define SCREEN_CENTER_Y (SCREEN_HEIGHT / 2)

struct Config {
    f32 audioFrequency;
#ifdef WIDE
    s16 widescreen;
#endif
    u8 tvType;
};

typedef struct Controller {
    /*0x00*/ s16 rawStickX;               // Analog stick [-128, 128] positive is right. Used for menus.
    /*0x02*/ s16 rawStickY;               // Analog stick [-128, 128] positive is up. Used for menus.
    /*0x04*/ f32 stickX;                  // Analog stick [-64, 64] positive is right. Used for gameplay.
    /*0x08*/ f32 stickY;                  // Analog stick [-64, 64] positive is up. Used for gameplay.
    /*0x0C*/ f32 stickMag;                // Analog stick distance from center [0, 64]. Used for gameplay.
    /*0x10*/ u16 buttonDown;              // Buttons held down on the current frame.
    /*0x12*/ u16 buttonPressed;           // Buttons pressed on the current frame but not held on the previous frame.
    /*0x14*/ u16 buttonReleased;          // Burrons released on the current frame and held on the previous frame.
    /*0x18*/ OSContStatus* statusData;    // Pointer to the controller status data in gControllerStatuses.
    /*0x1C*/ OSContPadEx* controllerData; // Pointer to the raw input data in gControllerPads.
    /*0x20*/ s32 port;                    // The port index this controller is plugged into [0, 3].
} Controller; /*0x24*/

// -- Booleans --

typedef u8  Bool8;
typedef u16 Bool16;
typedef u32 Bool32;

// -- Vectors --

typedef u8 Vec2uc[2];
typedef s8  Vec2c[2];
typedef s16 Vec2s[2];
typedef s32 Vec2i[2];
typedef f32 Vec2f[2];
typedef f64 Vec2d[2];

typedef u8 Vec3uc[3];
typedef s8  Vec3c[3];
typedef s16 Vec3s[3];
typedef s32 Vec3i[3];
typedef f32 Vec3f[3]; // X, Y, Z, where Y is up
typedef f64 Vec3d[3];

typedef u8 Vec4uc[4];
typedef s8  Vec4c[4];
typedef s16 Vec4s[4];
typedef s32 Vec4i[4];
typedef f32 Vec4f[4];
typedef f64 Vec4d[4];

typedef f32 Mat2[2][2];
typedef f32 Mat3[3][3];
typedef f32 Mat4[4][4];

// -- Scripts --
typedef uintptr_t GeoLayout;
typedef uintptr_t LevelScript;
typedef uintptr_t BehaviorScript;

// -- Mario/Objects --
typedef s32 MarioAction;
typedef s32 MarioActionArg;
typedef u32 MarioInteraction;
typedef u32 InteractType;
typedef u32 MarioStep;

typedef void (*ObjActionFunc)(void);

typedef s8  ObjAction8;
typedef s32 ObjAction32;
typedef s16 ColFlags;

// -- Collision --
typedef ROOM_DATA_TYPE RoomData;
typedef COLLISION_DATA_TYPE Collision; // Collision is by default an s16, but it's best to have it match the type of COLLISION_DATA_TYPE
typedef Collision TerrainData;
typedef Collision Vec3t[3];
typedef Collision SurfaceType;

// -- Colors/Textures --

// 0.0f to 1.0f
typedef f32    ColorF;
typedef ColorF AlphaF;
typedef ColorF ColorRGBf[3];
typedef ColorF ColorRGBAf[4];
typedef ColorF ColorHSVf[3];
typedef ColorF ColorHSVAf[4];

// 0 to 255
typedef u8    Color;
typedef Color Alpha;
typedef Color ColorRGB[3];
typedef Color ColorRGBA[4];
typedef Color ColorHSV[3];
typedef Color ColorHSVA[4];

// Formats:
typedef u32   CompositeColor;  // to be casted to one of the following:
typedef u8    I4;              // u4
typedef u8    I8;
typedef u8    IA4;             // u4, components u3 & u1
typedef u8    IA8;
typedef u8    IA16Component;
typedef u16   IA16;
typedef u8    RGBA16Component; // components u5 & u1
typedef u16   RGBA16;
typedef u32   RGBA16Return32;
typedef u32   RGBA16FILL;      // RGBA16 but twice, Used for gDPSetFillColor
typedef u8    RGBA32Component;
typedef u32   RGBA32;
typedef u8    CI4;             // u4
typedef u8    CI8;
typedef Color Texture;
typedef u32   Texture32;

typedef s16   TextureCoord;

// -- Models --

typedef u8  ModelID8;
typedef u16 ModelID16;
typedef u32 ModelID32;

// -- Animations --
typedef s16 AnimValue;
typedef u16 AnimIndex;
typedef s16 AnimID16;
typedef s32 AnimID32;
typedef s16 AnimFrame16;
typedef s32 AnimFrame32;
typedef s32 AnimAccel;

// -- Misc. Data --

typedef s16 DialogID16;
typedef s32 DialogID;
typedef s32 DrawingLayer;
typedef s16 PaintingData;
typedef s32 CameraTransitionAngle;
typedef s16 Movtex;
typedef s16 MacroObject;
typedef s16 Trajectory;
typedef u8  CutsceneID;

typedef u8 uchar;

enum SpTaskState {
    SPTASK_STATE_NOT_STARTED,
    SPTASK_STATE_RUNNING,
    SPTASK_STATE_INTERRUPTED,
    SPTASK_STATE_FINISHED,
    SPTASK_STATE_FINISHED_DP
};

struct SPTask {
    /*0x00*/ OSTask task;
    /*0x40*/ OSMesgQueue *msgqueue;
    /*0x44*/ OSMesg msg;
    /*0x48*/ enum SpTaskState state;
}; // size = 0x4C, align = 0x8

struct VblankHandler {
    OSMesgQueue *queue;
    OSMesg msg;
};

enum AnimFlags {
    ANIM_FLAG_NOLOOP     = BIT(0), // 0x01
    ANIM_FLAG_FORWARD    = BIT(1), // 0x02
    ANIM_FLAG_NO_ACCEL   = BIT(2), // 0x04
    ANIM_FLAG_HOR_TRANS  = BIT(3), // 0x08
    ANIM_FLAG_VERT_TRANS = BIT(4), // 0x10
    ANIM_FLAG_DISABLED   = BIT(5), // 0x20
    ANIM_FLAG_NO_TRANS   = BIT(6), // 0x40
    ANIM_FLAG_UNUSED     = BIT(7), // 0x80
};

struct Animation {
    /*0x00*/ s16 flags;
    /*0x02*/ s16 animYTransDivisor;
    /*0x04*/ s16 startFrame;
    /*0x06*/ s16 loopStart;
    /*0x08*/ s16 loopEnd;
    /*0x0A*/ s16 unusedBoneCount;
    /*0x0C*/ const s16 *values;
    /*0x10*/ const u16 *index;
    /*0x14*/ u32 length; // only used with Mario animations to determine how much to load. 0 otherwise.
};

#define ANIMINDEX_NUMPARTS(animindex) (sizeof(animindex) / sizeof(u16) / 6 - 1)

struct GraphNode {
    /*0x00*/ s16 type; // structure type
    /*0x02*/ s16 flags; // hi = drawing layer, lo = rendering modes
    /*0x04*/ struct GraphNode *prev;
    /*0x08*/ struct GraphNode *next;
    /*0x0C*/ struct GraphNode *parent;
    /*0x10*/ struct GraphNode *children;
};

struct AnimInfo {
    /*0x00 0x38*/ s16 animID;
    /*0x02 0x3A*/ s16 animYTrans;
    /*0x04 0x3C*/ struct Animation *curAnim;
    /*0x08 0x40*/ s16 animFrame;
    /*0x0A 0x42*/ u16 animTimer;
    /*0x0C 0x44*/ s32 animFrameAccelAssist;
    /*0x10 0x48*/ s32 animAccel;
};

struct GraphNodeObject {
    /*0x00*/ struct GraphNode node;
    /*0x14*/ struct GraphNode *sharedChild;
    /*0x18*/ s8 areaIndex;
    /*0x19*/ s8 activeAreaIndex;
    /*0x1A*/ Vec3s angle;
    /*0x20*/ Vec3f pos;
    /*0x2C*/ Vec3f scale;
    /*0x38*/ struct AnimInfo animInfo;
    /*0x4C*/ struct SpawnInfo *spawnInfo;
    /*0x50*/ Mat4 *throwMatrix; // matrix ptr
    /*0x54*/ Vec3f cameraToObject;
#ifdef OBJECTS_REJ
    u16 ucode;
#endif
};

struct ObjectNode {
    struct GraphNodeObject gfx;
    struct ObjectNode *next;
    struct ObjectNode *prev;
};

// NOTE: Since ObjectNode is the first member of Object, it is difficult to determine
// whether some of these pointers point to ObjectNode or Object.

#ifdef PUPPYLIGHTS
#define MAX_OBJECT_FIELDS 0x51
#else
#define MAX_OBJECT_FIELDS 0x50
#endif

struct Object {
    /*0x000*/ struct ObjectNode header;
    /*0x068*/ struct Object *parentObj;
    /*0x06C*/ struct Object *prevObj;
              struct Object *nextPhysicsObj;
    /*0x070*/ u32 collidedObjInteractTypes;
    /*0x074*/ s16 activeFlags;
    /*0x076*/ s16 numCollidedObjs;
    /*0x078*/ struct Object *collidedObjs[4];
    /*0x088*/
    union {
        // Object fields. See object_fields.h.
        u32 asU32[MAX_OBJECT_FIELDS];
        s32 asS32[MAX_OBJECT_FIELDS];
        s16 asS16[MAX_OBJECT_FIELDS][2];
        f32 asF32[MAX_OBJECT_FIELDS];
        s16 *asS16P[MAX_OBJECT_FIELDS];
        s32 *asS32P[MAX_OBJECT_FIELDS];
        struct Animation **asAnims[MAX_OBJECT_FIELDS];
        struct Waypoint *asWaypoint[MAX_OBJECT_FIELDS];
        struct ChainSegment *asChainSegment[MAX_OBJECT_FIELDS];
        struct Object *asObject[MAX_OBJECT_FIELDS];
        struct Surface *asSurface[MAX_OBJECT_FIELDS];
        void *asVoidPtr[MAX_OBJECT_FIELDS];
        const void *asConstVoidPtr[MAX_OBJECT_FIELDS];
    } rawData;
    // /*0x1C8*/ struct RigidBody *rigidBody;
    /*0x1CC*/ const BehaviorScript *curBhvCommand;
    /*0x1D0*/ u32 bhvStackIndex;
    /*0x1D4*/ uintptr_t bhvStack[3];
    /*0x1F4*/ s16 bhvDelayTimer;
    /*0x1F6*/ s16 respawnInfoType;
              u8 oQuicksandDepth;
              u8 oQuicksandDepthToDie; // 0 means wont sink
              u8 oHasPhysics; // Don't get mad. Need this because when objects get deleted, their flags get wiped.
              u8 oImbue; // Hey, look at that. 4 bytes like an f32, very convenient!
    /*0x1F8*/ f32 hitboxRadius;
    /*0x1FC*/ f32 hitboxHeight;
    /*0x200*/ f32 hurtboxRadius;
    /*0x204*/ f32 hurtboxHeight;
    /*0x208*/ f32 hitboxDownOffset;
    /*0x20C*/ const BehaviorScript *behavior;
    // /*0x214*/ struct Object *platform;
    /*0x218*/ void *collisionData;
    /*0x21C*/ Mat4 transform;
    /*0x25C*/ void *respawnInfo;
              Vec3f oDisplaceVec;
              Vec3f oOldVec;
#ifdef PUPPYLIGHTS
    struct PuppyLight puppylight;
#endif
};

struct ObjectHitbox {
    /*0x00*/ u32 interactType;
    /*0x04*/ u8 downOffset;
    /*0x05*/ s8 damageOrCoinValue;
    /*0x06*/ s8 health;
    /*0x07*/ s8 numLootCoins;
    /*0x08*/ s16 radius;
    /*0x0A*/ s16 height;
    /*0x0C*/ s16 hurtboxRadius;
    /*0x0E*/ s16 hurtboxHeight;
};

struct Waypoint {
    s16 flags;
    Vec3s pos;
};

struct Normal {
    /*0x00*/ f32 x;
    /*0x04*/ f32 y;
    /*0x08*/ f32 z;
};

struct Surface {
    TerrainData type;
    s16 lowerY;
    s16 upperY;
    Vec3t vertex1;
    Vec3t vertex2;
    Vec3t vertex3;
    struct Object *object;
};

#define PUNCH_STATE_TIMER_MASK          0b00111111
#define PUNCH_STATE_TYPES_MASK          0b11000000

enum PunchStateTypes {
    PUNCH_STATE_TYPE_FIRST_PUNCH  = (0 << 6),
    PUNCH_STATE_TYPE_SECOND_PUNCH = (1 << 6),
    PUNCH_STATE_TYPE_KICK         = (2 << 6),
};

struct MarioBodyState {
    /*0x00*/ u32 action;
    /*0x04*/ s8 capState; /// see MarioCapGSCId
    /*0x05*/ s8 eyeState;
    /*0x06*/ s8 handState;
    /*0x07*/ s8 wingFlutter; /// whether Mario's wing cap wings are fluttering
    /*0x08*/ s16 modelState;
    /*0x0A*/ s8 grabPos;
    /*0x0B*/ u8 punchState; /// 2 bits for type of punch, 6 bits for punch animation timer
    /*0x0C*/ Vec3s torsoAngle;
    /*0x12*/ Vec3s headAngle;
    /*0x18*/ Vec3f heldObjLastPosition; /// also known as HOLP
    // u8 filler[4];
};

struct MarioState {
    /*0x00*/ u16 playerID;
    /*0x02*/ u16 input;
    /*0x04*/ u32 flags;
    /*0x08*/ u32 particleFlags;
    /*0x0C*/ u32 action;
    /*0x10*/ u32 prevAction;
    /*0x14*/ u32 terrainSoundAddend;
    /*0x18*/ u16 actionState;
    /*0x1A*/ u16 actionTimer;
    /*0x1C*/ u32 actionArg;
    /*0x20*/ f32 intendedMag;
    /*0x24*/ s16 intendedYaw;
    /*0x26*/ s16 invincTimer;
    /*0x28*/ u8 framesSinceA;
    /*0x29*/ u8 framesSinceB;
    /*0x2A*/ u8 wallKickTimer;
    /*0x2B*/ u8 doubleJumpTimer;
    /*0x2C*/ Vec3s faceAngle;
    /*0x32*/ Vec3s angleVel;
    /*0x38*/ s16 slideYaw;
    /*0x3A*/ s16 twirlYaw;
    /*0x3C*/ Vec3f pos;
    /*0x48*/ Vec3f vel;
    /*0x54*/ f32 forwardVel;
    /*0x58*/ f32 slideVelX;
    /*0x5C*/ f32 slideVelZ;
    /*0x60*/ struct Surface *wall;
    /*0x64*/ struct Surface *ceil;
    /*0x68*/ struct Surface *floor;
    Vec3f floorNormal;
    /*0x6C*/ f32 ceilHeight;
    /*0x70*/ f32 floorHeight;
    /*0x74*/ s16 floorYaw;
    #define floorAngle floorYaw
    /*0x76*/ s16 waterLevel;
    /*0x78*/ struct Object *interactObj;
    /*0x7C*/ struct Object *heldObj;
             u8 heldObjParam2;
    /*0x80*/ struct Object *usedObj;
    /*0x84*/ struct Object *riddenObj;
    /*0x88*/ struct Object *marioObj;
    /*0x8C*/ struct SpawnInfo *spawnInfo;
    /*0x90*/ struct Area *area;
    /*0x94*/ struct PlayerCameraState *statusForCamera;
    /*0x98*/ struct MarioBodyState *marioBodyState;
    /*0x9C*/ struct Controller *controller;
    /*0xA0*/ struct DmaHandlerList *animList;
    /*0xA4*/ u32 collidedObjInteractTypes;
    /*0xA8*/ s16 numCoins;

            f32 PortalTint;
            u8 hundredSpawned;

            f32 waterBottomHeight;
            u16 waterBottomParam;

            u8 _2D;
            u8 _2D_Setting;

            struct Object *faceCrablet;
            //crimas only!
            u8 MessHeld;
            u8 MessNumber;
            u8 IntroDid;
            //crimas only!

             u16 numGlobalCoins;
             u16 numMaxGlobalCoins;
             u8 LastCostumeID;
             u8 CostumeID;
             u8 numMaxHP;
             u8 numMaxFP;
             u8 numMaxBP;

             s16 numAir;

             Vec3f StarRadarLocation;
             u8 StarRadarExist;

             s8 numBadgeSelect;
             u32 EquippedBadges;
            u8 numEquippedBadges;

             u8 numFakeHP;
             u8 numFakeHPdamage;
             u8 numFakeHPheal;

             u8 TokenParam2;
             u8 MenuToRender;
            
            s32 ScreenPosY;
            s32 ScreenPosX;

            u8 OptionMusic;
            u8 OptionWidescreen;

            s16 SwitchPressed;

             s16 numBadgePoints;
    /*0xAA*/ s16 numStars;
             s16 numMetalStars;
             s16 lastStarCollected;
             u16 NewTimer;
             u8 NewTimerMode;

            u8 NewLevel;

             s16 SubNewTimer;
             s16 GoldRingCount;
             s16 PirCount;

            u8 YoshiCoins;
            u8 DeadRexes;
            u8 DeadRexMissionActivate;
            u8 DeadCowboyMissionActivate;
            u8 DeadPokeys;
            u8 CheeseCollection;
            u8 CheeseMissionActivate;
            u8 SockCollection;
            u8 SockMissionActivate;

            u8 RFuel;
            u8 SFuel;

            u8 Yoshi_Flutter;
            u8 IsYoshi;

             u16 toggleHud;

            u8 BadAppleActivate;
            u8 GlobalPaused;

            u8 gCurrMinigame;
            u8 gMinigameWon;
            u8 EA_TOTAL;
            u8 EA_LEFT;
            u8 EA_ACTIVE;
            u8 EA_WAVES;

            u16 hiddenBoxTimer;
            u16 blueCoinSwitchTimer; // im so evil

    /*0xAC*/ s8 numKeys; // Unused key mechanic
    /*0xAD*/ s8 numLives;
             s16 gGlobalCoinGain;
    /*0xAE*/ s16 health;
    /*0xB0*/ s16 animYTrans;
             s8 powerup;
    /*0xB2*/ u8 hurtCounter;
    /*0xB3*/ u8 healCounter;
    /*0xB4*/ u8 squishTimer;
    /*0xB5*/ u8 fadeWarpOpacity;
    /*0xB6*/ u16 capTimer;
    /*0xB8*/ s16 prevNumStarsForDialog;
    /*0xBC*/ f32 peakHeight;
    /*0xC0*/ f32 quicksandDepth;
    /*0xC4*/ f32 windGravity;
             u8 ShopID;
             u8 Level;
            u8 ISSAFE;
            u8 _2DSecret;

            s8 BossHealth;
            s8 BossHealthMax;

            u8 TollPaid;
            Vec3f posDelay;

            f32 _2D_FOV_PUBLIC;

            u8 MaskChase;

            u8 LavaHeat;

            u8 Cheats;
            u8 Options;

            u8 nearVendor;
            u8 onbluecoinswitch;
            u8 isAfterlife;

            f32 gravMult;

            struct Object *SpotlightTarget;
            f32 SpotlightTargetYOffset;
    // -- HackerSM64 MarioState fields begin --
#ifdef BREATH_METER
             s16 breath;
             u8  breathCounter;
#endif
#ifdef PREVENT_DEATH_LOOP
             u8  isDead : 1;
#endif
           Vec3f lastSafePos;
           Vec3f prevPos;
             f32 lateralSpeed;
             f32 moveSpeed;
             s16 movePitch;
             s16 moveYaw;
             s16 ceilYaw;
             s16 wallYaw;
    // -- HackerSM64 MarioState fields end --
};

#endif // TYPES_H
