#include <string.h>

#include "main.h"
#include "cmsis_os.h"

#include "flexible_button.h"
#include "key.h"

#define ENUM_TO_STR(e) (#e)

char *enum_btn_event_string[] = {
    ENUM_TO_STR(BTN_DOWN),
    ENUM_TO_STR(BTN_CLICK),
    ENUM_TO_STR(BTN_DOUBLE_CLICK),
    ENUM_TO_STR(BTN_REPEAT_CLICK),
    ENUM_TO_STR(BTN_SHORT_START),
    ENUM_TO_STR(BTN_SHORT_UP),
    ENUM_TO_STR(BTN_LONG_START),
    ENUM_TO_STR(BTN_LONG_UP),
    ENUM_TO_STR(BTN_LONG_HOLD_START),
    ENUM_TO_STR(BTN_LONG_HOLD_UP),
    ENUM_TO_STR(BTN_MAX),
    ENUM_TO_STR(BTN_NONE),
};

key_evt_val_queue_t key_evt_val = 
{
    .key_id = USER_BUTTON_MAX,
    .key_evt = BTN_NONE,
};

QueueHandle_t Queue_key_evt_val_handle;
 
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

void key1_evt_process_task(void const * argument)
{    
  for(;;)
  {
    key_evt_val_queue_t key_evt_val_consumer = 
    {
        .key_id = USER_BUTTON_MAX,
        .key_evt = BTN_NONE,
    };

	UBaseType_t msgCount = uxQueueMessagesWaiting(Queue_key_evt_val_handle);
	UBaseType_t freeSpace = uxQueueSpacesAvailable(Queue_key_evt_val_handle);

	BaseType_t result = xQueueReceive(Queue_key_evt_val_handle, &key_evt_val_consumer, pdMS_TO_TICKS(100));
	
	if(result != pdTRUE)
		continue;
	
	printf("msgCount: %d, freeSpace: %d, key_id:%d, key_evt:%d\r\n", (uint16_t)msgCount, (uint16_t)freeSpace, key_evt_val_consumer.key_id, key_evt_val_consumer.key_evt);

    switch (key_evt_val_consumer.key_id)
    {
        case USER_BUTTON_0:
        {            
            switch (key_evt_val_consumer.key_evt)
            {
                case BTN_CLICK:
                {
                    printf("key click!\r\n");
                }
                break;
                case BTN_DOUBLE_CLICK:
                {
                    printf("double click!\r\n");
                }
                break;
                case BTN_REPEAT_CLICK:
                {
                    printf("repeat click!\r\n");
                }
                break;								
                case BTN_SHORT_UP:
                {
                    printf("key press short!\r\n");
                }
                break;
                case BTN_LONG_UP:
                {
                    printf("key press long!\r\n");
                }
                break;
                case BTN_LONG_HOLD_START:
                {
                    printf("key long hold!\r\n");
                }
                break;

                default:
                    break;
            }
        }
        break;
    }

    osDelay(100);

  }
}

static void vKeysEvtCb(void *arg)
{
    flex_button_t *btn = (flex_button_t *)arg;
    printf("id: [%d]  event: [%d - %20s]  repeat: %d\r\n",
           btn->id, btn->event, enum_btn_event_string[btn->event], btn->click_cnt);

    key_evt_val.key_id = btn->id;
    key_evt_val.key_evt = btn->event;
    if(pdPASS != xQueueSendToBack(Queue_key_evt_val_handle, (void *)&key_evt_val, pdMS_TO_TICKS(100)))
    {
        printf("send key evt vlaue to queue failed\r\n");
        return;
    }
}

static uint8_t u8Key1Read(void *arg)
{
  return (uint8_t)HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
}

void key_init(void)
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

    Queue_key_evt_val_handle = xQueueCreate(5, sizeof(uint16_t));

    //Key event sampling task
    osThreadDef(key1_evt_scan, key1_scan_task, osPriorityAboveNormal, 0, 1024);
    osThreadId key1_scan_task = osThreadCreate(osThread(key1_evt_scan), NULL); 

    //Key event process task
    osThreadDef(key1_evt_process, key1_evt_process_task, osPriorityAboveNormal, 0, 1024);
    osThreadId key1_evt_process_task = osThreadCreate(osThread(key1_evt_process), NULL);  
}

