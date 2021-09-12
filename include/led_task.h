#ifndef _LED_TASK_H_
#define _LED_TASK_H_

#define LED_GPIO 13

TaskHandle_t xLED_task = NULL;

void LED_task(void *pvParameter);

#endif /* _LED_TASK_H_ */