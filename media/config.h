#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "duerapp_kbd_listener.h"
#include "event.h"
#include "queue.h"
#include "duerapp_media.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#ifndef bool
typedef char bool;
#define true (1)
#define false (0)
#endif

#define DUER_LOGI(...)
#define DUER_LOGE(...)

#endif // _CONFIG_H_
