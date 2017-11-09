// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_config.h
 * Auth: Su Hao (suhao@baidu.com)
 * Desc: Duer Configuration.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_CONFIG_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_CONFIG_H

#include <stdbool.h>

#include "lightduer_log.h"

/*
 * Load profile
 */

typedef enum{
    PLAY_IDLE,
    PLAY_SPEAK,
    PLAY_AUDIO
}duer_play_type_t;

const char *duer_load_profile(const char *path);

#endif // BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_CONFIG_H
