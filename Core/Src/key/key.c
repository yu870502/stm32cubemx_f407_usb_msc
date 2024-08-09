#include <string.h>

#include "main.h"
#include "cmsis_os.h"

#include "flexible_button.h"
#include "key.h"

#define ENUM_TO_STR(e) (#e)

char *enum_btn_event_string[] = {
    ENUM_TO_STR(FLEX_BTN_PRESS_DOWN),
    ENUM_TO_STR(FLEX_BTN_PRESS_CLICK),
    ENUM_TO_STR(FLEX_BTN_PRESS_DOUBLE_CLICK),
    ENUM_TO_STR(FLEX_BTN_PRESS_REPEAT_CLICK),
    ENUM_TO_STR(FLEX_BTN_PRESS_SHORT_START),
    ENUM_TO_STR(FLEX_BTN_PRESS_SHORT_UP),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_START),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_UP),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_HOLD),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_HOLD_UP),
    ENUM_TO_STR(FLEX_BTN_PRESS_MAX),
    ENUM_TO_STR(FLEX_BTN_PRESS_NONE),
};

flex_button_t keyGroup[USER_BUTTON_MAX];

int key_scan_interval = 1000 / FLEX_BTN_SCAN_FREQ_HZ;
void key1_scan_task(void const * argument)
{
  for(;;)
  {
    flex_button_scan();
    osDelay(key_scan_interval);  //注意，这里间隔时间要与FLEX_BTN_SCAN_FREQ_HZ紧密联系
  }
}

void key_process_start()
{
  osThreadDef(key1_scan, key1_scan_task, osPriorityAboveNormal, 0, 1024);
  osThreadId key1_scanTaskHandle = osThreadCreate(osThread(key1_scan), NULL);    
}

static void vKeysEvtCb(void *arg)
{
    flex_button_t *btn = (flex_button_t *)arg;
    printf("id: [%d]  event: [%d - %30s]  repeat: %d\r\n",
           btn->id, btn->event, enum_btn_event_string[btn->event],
           btn->click_cnt);
    switch (btn->id)
    {
        case USER_BUTTON_0:
        {
            switch (btn->event)
            {

            case FLEX_BTN_PRESS_SHORT_UP:
            {
                printf("key press short!\r\n");
            }
            break;

            case FLEX_BTN_PRESS_LONG_UP:
            {
                printf("key press long!\r\n");
            }
            break;

            case FLEX_BTN_PRESS_CLICK:
            {
                printf("key click!\r\n");
            }
            break;

            case FLEX_BTN_PRESS_LONG_HOLD:
            {
                printf("key long hold!\r\n");
            }
            break;

            case FLEX_BTN_PRESS_DOUBLE_CLICK:
            {
                printf("double click!\r\n");
            }
            break;

            case FLEX_BTN_PRESS_REPEAT_CLICK:
            {
                printf("repeat click!\r\n");
            }
            break;

            default:
                break;
            }
        }
        break;
    }
}

static uint8_t u8Key1Read(void *arg)
{
  return (uint8_t)HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
}

void register_key(void)
{
	int i = 0;
  memset(keyGroup, 0, sizeof(keyGroup));
  for (i = 0; i < USER_BUTTON_MAX; i++)
  {
      keyGroup[i].id = i;
      keyGroup[i].usr_button_read = u8Key1Read;
      keyGroup[i].cb = vKeysEvtCb;
      keyGroup[i].pressed_logic_level = 0;
      keyGroup[i].debounce_tick = 20;
      keyGroup[i].max_multiple_clicks_interval = FLEX_MS_TO_SCAN_CNT(500); // 单击间隔500ms
      keyGroup[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500);
      keyGroup[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);
      keyGroup[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(5000);
      flex_button_register(&keyGroup[i]);
  }  
}

