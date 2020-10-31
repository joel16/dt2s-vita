/* dt2s.c -- Double Tap 2 Sleep for PS Vita
 *
 * Copyright (C) 2020 Joel16
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <psp2/ctrl.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/power.h>
#include <psp2/touch.h>
#include <psp2/types.h>
#include <taihen.h>

// Checks whether a result code indicates failure.
#define R_FAILED(res) ((res) < 0)

#define lerp(value, from_max, to_max) ((((value * 10) * (to_max * 10)) / (from_max * 10)) / 10)

static SceUID thread_id = 0;
static SceBool running = SCE_TRUE;
static const SceUInt64 delay_period = 500000;
static const int px = 15;
static const int py = 15;

typedef struct {
    SceBool pressed;
    SceBool pressed_prev;
    SceUInt64 old_timestamp;
    int pos_x;
    int pos_y;
    int old_pos_x;
    int old_pos_y;
} TouchState;

static SceBool dt2s_logic(SceTouchData *touch, TouchState *touch_state) {
#ifdef DT2S_STATUS_BAR
    if (touch_state->pos_y > 40) // The height is about ~32 pixels but lets use 40 for our sake.
        return SCE_FALSE;
#endif
    
    if (!touch_state->old_timestamp) {
        touch_state->old_timestamp = touch->timeStamp;
        touch_state->old_pos_x = touch_state->pos_x;
        touch_state->old_pos_y = touch_state->pos_y;
        return SCE_FALSE;
    }
    else {
        if (((touch->timeStamp - touch_state->old_timestamp) <= delay_period) &&
            (((touch_state->old_pos_x - touch_state->pos_x) >= -px) && ((touch_state->old_pos_x - touch_state->pos_x) <= px)) &&
            (((touch_state->old_pos_y - touch_state->pos_y) >= -py) && ((touch_state->old_pos_y - touch_state->pos_y) <= py))) {
            touch_state->old_timestamp = 0;
            return SCE_TRUE;
        }
        else {
            touch_state->old_timestamp = touch->timeStamp;
            touch_state->old_pos_x = touch_state->pos_x;
            touch_state->old_pos_y = touch_state->pos_y;
            return SCE_FALSE;
        }
    }
}

static int dt2s_thread(SceSize args, void *argp) {
    SceTouchData touch;
    TouchState touch_state;
    
    sceClibMemset(&touch, 0, sizeof(SceTouchData));
    sceClibMemset(&touch_state, 0, sizeof(TouchState));

    while (running) {
        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
        
        if (touch.reportNum > 0) {
            touch_state.pos_x = (lerp(touch.report[0].x, 1919, 960));
            touch_state.pos_y = (lerp(touch.report[0].y, 1087, 544));

            if (touch_state.pressed_prev == SCE_FALSE) {
                touch_state.pressed_prev = SCE_TRUE;
                touch_state.pressed = SCE_TRUE;
            }
            else
                touch_state.pressed = SCE_FALSE;
        }
        else {
            touch_state.pressed = SCE_FALSE;
            touch_state.pressed_prev = SCE_FALSE;
        }
        
        if (touch_state.pressed) {
            if (dt2s_logic(&touch, &touch_state))
                scePowerRequestSuspend();
        }
        
        sceKernelDelayThread(50 * 1000);
    }
    
    return 0;
}

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp) {
    int ret = 0;
    SceTouchSamplingState sampling_state;
    
    if (R_FAILED(ret = sceTouchGetSamplingState(SCE_TOUCH_PORT_FRONT, &sampling_state)))
        return ret;
        
    if (sampling_state == SCE_TOUCH_SAMPLING_STATE_STOP) {
        if (R_FAILED(ret = sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START)))
            return ret;
    }
    
    thread_id = sceKernelCreateThread("dt2s", dt2s_thread, 0x40, 0x40000, 0, 0, NULL);
    sceKernelStartThread(thread_id, 0, NULL);
    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, void *argp) {
    if (thread_id >= 0) {
        running = SCE_FALSE;
        sceKernelWaitThreadEnd(thread_id, NULL, NULL);
        sceKernelDeleteThread(thread_id);
    }
    
    return SCE_KERNEL_STOP_SUCCESS;
}
