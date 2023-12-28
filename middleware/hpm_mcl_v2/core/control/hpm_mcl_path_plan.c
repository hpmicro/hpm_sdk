/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_mcl_path_plan.h"

hpm_mcl_stat_t hpm_mcl_path_init(mcl_path_plan_t *path, mcl_path_plan_cfg_t *cfg, mcl_cfg_t *mcl)
{
    MCL_ASSERT(path != NULL, mcl_invalid_pointer);
    MCL_ASSERT(cfg != NULL, mcl_invalid_pointer);
    MCL_ASSERT(mcl != NULL, mcl_invalid_pointer);
    path->pole_num = &mcl->physical.motor.pole_num;
    path->cfg = cfg;
    path->t_cure.next_empty = true;
    path->t_cure.current = cfg->t_cure;
    path->t_cure.ts = 0;
    path->t_cure.current_empty = true;

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_path_update_t_cure(mcl_path_plan_t *path, path_plan_t_cure_cfg_t *cfg)
{
    MCL_ASSERT_OPT(path != NULL, mcl_invalid_pointer);

    if (path->t_cure.current_empty) {
        path->t_cure.current_empty = false;
        path->t_cure.current = *cfg;
    } else if (path->t_cure.next_empty) {
        path->t_cure.next_empty = false;
        path->t_cure.next = *cfg;
    } else {
        return mcl_fail;
    }

    return mcl_success;
}

float hpm_mcl_path_get_current_theta(mcl_path_plan_t *path)
{
    return path->theta.current;
}

float hpm_mcl_path_get_next_theta(mcl_path_plan_t *path)
{
    return path->theta.next;
}

float hpm_mcl_path_get_last_theta(mcl_path_plan_t *path)
{
    return path->theta.last;
}

bool hpm_mcl_path_t_cure_complete(mcl_path_plan_t *path)
{
    if ((path->t_cure.current_empty ==  true) &&
        (path->t_cure.next_empty ==  true)) {
        return true;
    } else {
        return false;
    }
}

hpm_mcl_stat_t hpm_mcl_path_t_cure_generate(mcl_path_plan_t *path)
{
    float t0, t1, t2;
    int32_t num0, num1, num2;
    float theta;

    MCL_ASSERT_OPT(path != NULL, mcl_invalid_pointer);
    MCL_ASSERT_OPT(path->cfg->loop_ts != 0, mcl_invalid_argument);
    if (path->t_cure.current_empty) {
        path->t_cure.ts = 0;
        return mcl_success;
    }
    if (path->t_cure.ts == 0) {
        if (path->t_cure.current.time != 0) {
            t0 = path->t_cure.current.acc_time;
            t2 = path->t_cure.current.dec_time;
            t1 = path->t_cure.current.time - t0 - t2;
            num0 = t0 / path->cfg->loop_ts;
            num1 = t1 / path->cfg->loop_ts;
            num2 = t2 / path->cfg->loop_ts;

            path->t_cure.t0 = num0;
            path->t_cure.t1 = num0 + num1;
            path->t_cure.t2 = num0 + num1 + num2;
            path->t_cure.a0 = path->t_cure.current.acc * path->cfg->loop_ts * 0.5f;
            path->t_cure.a1 = ((path->t_cure.current.speed * t1) / num1);
            path->t_cure.a2 = path->t_cure.current.dec * path->cfg->loop_ts * 0.5f;
        } else {
            path->t_cure.t0 = 0;
            path->t_cure.t1 = 0;
            path->t_cure.t2 = 0;
            path->t_cure.a0 = 0;
            path->t_cure.a1 = 0;
            path->t_cure.a2 = 0;
        }
    }
    path->theta.last = path->theta.current;
    path->theta.current = path->theta.next;
    path->t_cure.ts += 1;

    if (path->t_cure.ts > path->t_cure.t2) {
        theta = path->theta.current;
        if (path->t_cure.next_empty == false) {
            path->t_cure.current = path->t_cure.next;
            path->t_cure.next_empty = true;
            path->t_cure.ts = 0;
        } else {
            path->t_cure.current_empty = true;
            path->t_cure.current.time = 0;
            path->t_cure.ts = 0;
        }
    } else if (path->t_cure.ts > path->t_cure.t1) {
        theta = path->t_cure.a2 * (path->cfg->loop_ts *(path->t_cure.t2 - path->t_cure.ts) * 2 - path->cfg->loop_ts);
        theta *= *path->pole_num;
        theta += path->theta.next;
    } else if (path->t_cure.ts > path->t_cure.t0) {
        theta = path->t_cure.a1;
        theta *= *path->pole_num;
        theta += path->theta.next;
    } else {
        theta = path->t_cure.a0 * (path->cfg->loop_ts * path->t_cure.ts * 2 - path->cfg->loop_ts);
        theta *= *path->pole_num;
        theta += path->theta.next;
    }
    path->theta.next = MCL_ANGLE_MOD_X(0, MCL_PI * 2, theta);

    return mcl_success;
}
