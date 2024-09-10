#ifndef __KEY_H__
#define __KEY_H__

#include <stdio.h>

#include "flexible_button.h"

typedef enum
{
    USER_BUTTON_0 = 0,
    USER_BUTTON_MAX
} user_button_t;

typedef struct __key_evt_val_queue_st
{
    uint8_t key_id;
    uint8_t key_evt;
}key_evt_val_queue_t;

extern char *enum_btn_event_string[];

void print_key_test_str(void);

void key_init(void);

void key_process_start(void);

extern flex_button_t keyGroup[USER_BUTTON_MAX];
#endif

