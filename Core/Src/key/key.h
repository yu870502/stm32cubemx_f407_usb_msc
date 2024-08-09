#ifndef __KEY_H__
#define __KEY_H__

#include <stdio.h>

#include "flexible_button.h"

typedef enum
{
    USER_BUTTON_0 = 0,
    USER_BUTTON_MAX
} user_button_t;

extern char *enum_btn_event_string[];

void print_key_test_str(void);

void register_key(void);

void key_process_start(void);

extern flex_button_t keyGroup[USER_BUTTON_MAX];
#endif

