// king_bobomb.inc.c

// Copy of geo_update_projectile_pos_from_parent
Gfx *geo_update_held_mario_pos(s32 callContext, UNUSED struct GraphNode *node, Mat4 mtx) {
    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        if (obj->prevObj != NULL) {
            obj_update_pos_from_parent_transformation(mtx, obj->prevObj);
            obj_set_gfx_pos_from_pos(obj->prevObj);
        }
    }

    return NULL;
}

void bhv_bobomb_anchor_mario_loop(void) {
    common_anchor_mario_behavior(50.0f, 50.0f, INT_STATUS_MARIO_DROPPED_BY_OBJ);
}

void king_bobomb_act_inactive(void) { // act 0
    o->oForwardVel = 0.0f;
    o->oQuicksandDepthToDie = 0;

    if (o->oSubAction == KING_BOBOMB_SUB_ACT_INACTIVE_INIT) {
        if (o->oTimer == 0) {
            vec3f_copy(&o->oHomeVec, &o->oPosVec);

            cur_obj_become_intangible();
            gSecondCameraFocus = o;
            cur_obj_init_animation_with_sound(KING_BOBOMB_ANIM_IDLE);
            o->oHealth = 3;

            cur_obj_move_standard(-78);
        }
        cur_obj_die_if_on_death_barrier(MB64_STAR_HEIGHT);

        if (o->oDistanceToMario < MB64_BOSS_TRIGGER_DIST) {
            o->oSubAction++;
            //seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
        }
    } else {
        o->oPlayingBossMusic = TRUE;
        o->oAction = KING_BOBOMB_ACT_ACTIVE;
        o->oFlags |= OBJ_FLAG_HOLDABLE;
    }
}

s32 mario_is_far_below_object(f32 min) {
    return min < o->oPosY - gMarioObject->oPosY;
}

void king_bobomb_act_active(void) { // act 2
    cur_obj_become_tangible();

    if ((o->oFloorType == SURFACE_DEATH_PLANE) && (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND)) {
        o->oAction = KING_BOBOMB_ACT_RETURN_HOME;
    }

    if (o->oKingBobombShouldStomp == 0) {
        if (cur_obj_check_anim_frame(15)) {
            cur_obj_shake_screen(SHAKE_POS_SMALL);
        }

        if (cur_obj_init_animation_and_check_if_near_end(KING_BOBOMB_ANIM_STOMP)) {
            o->oKingBobombShouldStomp++;
        }
    } else {
        if (o->oKingBobombShouldStomp == 1) {
            cur_obj_init_animation_and_anim_frame(KING_BOBOMB_ANIM_WALKING, 7);
            o->oKingBobombShouldStomp = 2;
        } else {
            cur_obj_init_animation_with_sound(KING_BOBOMB_ANIM_WALKING);
        }

        if (o->oKingBobombPlayerGrabEscapeCooldown == 0) {
            o->oForwardVel = 3.0f;
            cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x100);
        } else {
            o->oForwardVel = 0.0f;
            o->oKingBobombPlayerGrabEscapeCooldown--;
        }
    }

    if (cur_obj_check_grabbed_mario()) {
        o->oAction = KING_BOBOMB_ACT_GRABBED_MARIO;
    } else if (o->oDistanceToMario > MB64_BOSS_DETRIGGER_DIST) {
        o->oAction = KING_BOBOMB_ACT_INACTIVE;
        o->oPlayingBossMusic = FALSE;
        o->oKingBobombShouldStomp = 0;
    }
}

void king_bobomb_act_grabbed_mario(void) { // act 3
    if (o->oSubAction == KING_BOBOMB_SUB_ACT_GRABBED_MARIO_GRAB) {
        o->oForwardVel = 0.0f;
        o->oKingBobombStationaryTimer = 0;
        o->oKingBobombPlayerGrabEscapeActions = 0;

        if (o->oTimer == 0) {
            cur_obj_play_sound_2(SOUND_OBJ_GRAB_MARIO);
        }

        if (cur_obj_init_animation_and_check_if_near_end(KING_BOBOMB_ANIM_GRAB_MARIO)) {
            o->oSubAction++;
            cur_obj_init_animation_and_anim_frame(KING_BOBOMB_ANIM_HOLDING_MARIO, 0);
        }
    } else if (o->oSubAction == KING_BOBOMB_SUB_ACT_GRABBED_MARIO_HOLDING) {
        cur_obj_init_animation_with_sound(KING_BOBOMB_ANIM_HOLDING_MARIO);
        o->oKingBobombPlayerGrabEscapeActions += player_performed_grab_escape_action();

        print_debug_bottom_up("%d", o->oKingBobombPlayerGrabEscapeActions);

        if (o->oKingBobombPlayerGrabEscapeActions > 10) {
            o->oKingBobombHoldingMarioState = HELD_DROPPED;
            o->oAction = KING_BOBOMB_ACT_ACTIVE;
            o->oKingBobombPlayerGrabEscapeCooldown = 35;
            o->oInteractStatus &= ~(INT_STATUS_GRABBED_MARIO);
        } else {
            o->oForwardVel = 3.0f;

            if (o->oKingBobombStationaryTimer > 20 && cur_obj_rotate_yaw_toward(0x0, 0x400)) {
                o->oSubAction++;
                cur_obj_init_animation_and_anim_frame(KING_BOBOMB_ANIM_THROW_MARIO, 22);
            }
        }

        o->oKingBobombStationaryTimer++;
    } else {
        cur_obj_init_animation_with_sound(KING_BOBOMB_ANIM_THROW_MARIO);

        if (cur_obj_check_anim_frame(31)) {
            o->oKingBobombHoldingMarioState = HELD_THROWN;
            cur_obj_play_sound_2(SOUND_OBJ_RELEASE_MARIO);
        } else if (cur_obj_check_if_near_animation_end()) {
            o->oAction = KING_BOBOMB_ACT_ACTIVATE;
            o->oInteractStatus &= ~(INT_STATUS_GRABBED_MARIO);
        }
    }
}

void king_bobomb_act_activate(void) { // act 1
    o->oForwardVel = 0.0f;
    o->oVelY = 0.0f;

    cur_obj_init_animation_with_sound(KING_BOBOMB_ANIM_WALKING);

    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x200);

    if (o->oDistanceToMario < MB64_BOSS_TRIGGER_DIST) {
        o->oPlayingBossMusic = TRUE;
        o->oAction = KING_BOBOMB_ACT_ACTIVE;
    }

    if (mario_is_far_below_object(1200.0f)) {
        o->oAction = KING_BOBOMB_ACT_INACTIVE;
        o->oPlayingBossMusic = FALSE;
    }
}

void king_bobomb_act_hit_ground(void) { // act 6
    if (o->oSubAction == KING_BOBOMB_SUB_ACT_HIT_GROUND_HIT) {
        if (o->oTimer == 0) {
            o->oKingBobombStationaryTimer = 0;

            cur_obj_play_sound_2(SOUND_OBJ_KING_BOBOMB);
            cur_obj_play_sound_2(SOUND_OBJ2_KING_BOBOMB_DAMAGE);
            cur_obj_shake_screen(SHAKE_POS_SMALL);
            spawn_mist_particles_variable(0, 0, 100.0f);

            o->oInteractType = INTERACT_DAMAGE;

            cur_obj_become_tangible();
        }

        if (cur_obj_init_animation_and_check_if_near_end(KING_BOBOMB_ANIM_HIT_GROUND)) {
            o->oKingBobombStationaryTimer++;
        }

        if (o->oKingBobombStationaryTimer > 3) {
            o->oSubAction++; // KING_BOBOMB_SUB_ACT_HIT_GROUND_STAND_UP
        }
    } else if (o->oSubAction == KING_BOBOMB_SUB_ACT_HIT_GROUND_STAND_UP) {
        if (cur_obj_init_animation_and_check_if_near_end(KING_BOBOMB_ANIM_STAND_UP)) {
            o->oSubAction++; // KING_BOBOMB_SUB_ACT_HIT_GROUND_START_WALKING
            o->oInteractType = INTERACT_GRABBABLE;

            cur_obj_become_intangible();
        }
    } else {
        cur_obj_init_animation_with_sound(KING_BOBOMB_ANIM_WALKING);

        if (cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x800)) {
            o->oAction = KING_BOBOMB_ACT_ACTIVE;
        }
    }
}

void king_bobomb_act_death(void) { // act 7
    cur_obj_init_animation_with_sound(KING_BOBOMB_ANIM_HIT_GROUND);
    cur_obj_set_home_if_safe();
    if (1) {
        create_sound_spawner(SOUND_OBJ_KING_WHOMP_DEATH);

        cur_obj_hide();
        cur_obj_become_intangible();

        spawn_mist_particles_variable(0, 0, 200.0f);
        spawn_triangle_break_particles(20, MODEL_DIRT_ANIMATION, 3.0f, TINY_DIRT_PARTICLE_ANIM_STATE_YELLOW);
        cur_obj_shake_screen(SHAKE_POS_SMALL);

        cur_obj_drop_imbued_object(MB64_STAR_HEIGHT);

        o->oAction = KING_BOBOMB_ACT_STOP_MUSIC;
    }
}

void king_bobomb_act_stop_music(void) { // act 8
    if (o->oTimer == 60) {
        o->oPlayingBossMusic = FALSE;
        mark_obj_for_deletion(o); //stop phantom triggering noteblock
    }
}

void king_bobomb_act_been_thrown(void) { // act 4
    if (o->oFloorType != SURFACE_DEATH_PLANE) { // not thrown off hill
        if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
            o->oHealth--;

            o->oForwardVel = 0.0f;
            o->oVelY = 0.0f;

            cur_obj_play_sound_2(SOUND_OBJ_KING_BOBOMB);

            o->oAction = o->oHealth ? KING_BOBOMB_ACT_HIT_GROUND : KING_BOBOMB_ACT_DEATH;
        }
    } else if (o->oSubAction == KING_BOBOMB_SUB_ACT_THROWN_FALL) {
        if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
            o->oForwardVel = 0.0f;
            o->oVelY = 0.0f;

            o->oAction = KING_BOBOMB_ACT_RETURN_HOME;
        }
    }
}

s32 arc_to_goal_pos(Vec3f a0, Vec3f a1, f32 yVel, f32 gravity) {
    f32 dx = a0[0] - a1[0];
    f32 dz = a0[2] - a1[2];
    f32 planarDist = sqrtf(sqr(dx) + sqr(dz));

    o->oMoveAngleYaw = atan2s(dz, dx);
    o->oVelY = yVel;
    o->oGravity = gravity;

    s32 time = -2.0f / o->oGravity * yVel - 1.0f;

    o->oForwardVel = planarDist / time;

    return time;
}

void king_bobomb_act_return_home(void) { // act 5
    switch (o->oSubAction) {
        case KING_BOBOMB_SUB_ACT_RETURN_HOME_JUMP:
            if (o->oTimer == 0) {
                cur_obj_play_sound_2(SOUND_OBJ_KING_BOBOMB_JUMP);
            }

            o->oKingBobombIsJumping = TRUE;

            cur_obj_init_animation_and_extend_if_at_end(KING_BOBOMB_ANIM_JUMP);
            o->oMoveAngleYaw = cur_obj_angle_to_home();

            if (o->oPosY < o->oHomeY) {
                o->oVelY = 100.0f;
            } else {
                arc_to_goal_pos(&o->oHomeX, &o->oPosVec, 100.0f, -4.0f);
                o->oSubAction++; // KING_BOBOMB_SUB_ACT_RETURN_HOME_LANDING
            }
            break;

        case KING_BOBOMB_SUB_ACT_RETURN_HOME_LANDING:
            cur_obj_init_animation_and_extend_if_at_end(KING_BOBOMB_ANIM_JUMP);

            if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
                //o->oPosY = o->oHomeY;
                o->oVelY = 0;
                o->oForwardVel = 0;
                o->oGravity = -4.0f;

                o->oKingBobombIsJumping = FALSE;

                cur_obj_init_animation_with_sound(KING_BOBOMB_ANIM_T_POSE);
                cur_obj_play_sound_2(SOUND_OBJ_KING_BOBOMB);
                cur_obj_shake_screen(SHAKE_POS_SMALL);

                o->oSubAction++; // KING_BOBOMB_SUB_ACT_RETURN_HOME_LANDING_END
            }

            if (cur_obj_die_if_on_death_barrier(MB64_STAR_HEIGHT)) {
                obj_drop_mario();
            }
            break;

        case KING_BOBOMB_SUB_ACT_RETURN_HOME_LANDING_END:
            if (cur_obj_init_animation_and_check_if_near_end(KING_BOBOMB_ANIM_T_POSE)) {
                o->oAction = KING_BOBOMB_ACT_ACTIVE;
            }
            break;
    }
}

void king_bobomb_airborne(void) { // act 9
    if (o->oForwardVel < 1.0f) {
        o->oForwardVel = 1.0f;
    }
    o->oInteractType = INTERACT_DAMAGE;
    if (o->oTimer == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_KING_BOBOMB_JUMP);
        o->oVelY = 60.0f;
    } else {
        if (o->oMoveFlags & (OBJ_MOVE_MASK_ON_GROUND)) {
            o->oMoveFlags = 0;
            o->oVelY = 0.0f;
            o->oAction = 2;
            o->oInteractType = INTERACT_GRABBABLE;
        }
    }
    o->oKingBobombIsJumping = FALSE;
    cur_obj_init_animation_and_extend_if_at_end(KING_BOBOMB_ANIM_JUMP);
}

ObjActionFunc sKingBobombActions[] = {
    king_bobomb_act_inactive,
    king_bobomb_act_activate,
    king_bobomb_act_active,
    king_bobomb_act_grabbed_mario,
    king_bobomb_act_been_thrown,
    king_bobomb_act_return_home,
    king_bobomb_act_hit_ground,
    king_bobomb_act_death,
    king_bobomb_act_stop_music,
    king_bobomb_airborne,
};

struct SoundState sKingBobombSoundStates[] = {
    { 0, 0, 0, NO_SOUND },
    { 1, 1, 20, SOUND_OBJ_KING_BOBOMB_POUNDING1_HIGHPRIO },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 15, -1, SOUND_OBJ_KING_BOBOMB_POUNDING1_HIGHPRIO },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 33, -1, SOUND_OBJ_KING_BOBOMB_POUNDING1_HIGHPRIO },
    { 0, 0, 0, NO_SOUND },
    { 1, 1, 15, SOUND_OBJ_KING_BOBOMB_POUNDING1_HIGHPRIO },
};

void king_bobomb_move(void) {
    cur_obj_update_floor_and_walls();
    cur_obj_set_home_if_safe();
    cur_obj_move_standard(-78);

    //if (is_obj_interacting_with_noteblock(0)) {
    //    if (o->oAction == KING_BOBOMB_ACT_ACTIVE || o->oAction == KING_BOBOMB_ACT_) {
    //        o->oVelY = 60.0f;
    //        o->oAction = 9;
    //        cur_obj_move_standard(-78);
    //    }
    //}

    cur_obj_call_action_function(sKingBobombActions);
    exec_anim_sound_state(sKingBobombSoundStates);
}

void bhv_king_bobomb_loop(void) {
    o->oInteractionSubtype |= INT_SUBTYPE_GRABS_MARIO;

    switch (o->oHeldState) {
        case HELD_FREE:
            king_bobomb_move();
            break;
        case HELD_HELD:
            cur_obj_unrender_set_action_and_anim(6, 1);
            break;
        case HELD_THROWN:
        case HELD_DROPPED:
            cur_obj_get_thrown_or_placed(20.0f, 50.0f, 4);
            cur_obj_become_intangible();
            o->oPosY += 20.0f;
            break;
    }

    o->oInteractStatus = INT_STATUS_NONE;
    curr_obj_random_blink(&o->oKingBobombBlinkTimer);
}
