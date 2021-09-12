#ifndef _UART_TASK_H_
#define _UART_TASK_H_

#include "esp_log.h"
#include "driver/uart.h"

#define TXD_PIN 1
#define RXD_PIN 3

static const char TAG_UART[] = "UART";

static const int RX_BUF_SIZE = 1024;
static const int TX_BUF_SIZE = 1024;

typedef struct
{
  uint8_t head[2];
  uint8_t com1;
  uint8_t unk1[6];
  uint8_t com2[3];
  uint8_t unk2;
  uint8_t in_temp;
  uint8_t unk3[9];
  uint8_t mode;
  uint8_t unk4;
  uint8_t fan;
  uint8_t unk5;
  uint8_t swing;
  uint8_t unk6;
  uint8_t on_off;
  uint8_t unk7;
  uint8_t extra;
  uint8_t unk8[3];
  uint8_t temp;
  uint8_t sum;
} AC_data;

uint8_t genCRC(AC_data *data);
void UART_init(void);
void sendData(const void *data, int len);
void readData(void);

#endif /* _UART_TASK_H_ */