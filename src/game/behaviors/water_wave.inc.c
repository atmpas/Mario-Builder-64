// water_wave.inc.c
// TODO: Rename to avoid confusion with water_splashes_and_waves

void bhv_object_water_wave_init(void) {
    o->oPosY = mb64_get_water_level(o->oPosX, o->oPosY, o->oPosZ);
}

void bhv_object_water_wave_loop(void) {
    s32 globalTimer = gGlobalTimer;
    if (!(globalTimer & 0x15)) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}
