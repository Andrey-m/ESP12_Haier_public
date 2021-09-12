#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "protocol.h"
#include "led_task.h"
#include "ir_task.h"
#include "uart_task.h"

static const char *TAG_MAIN = "Smart AC";

#define IR_RX_IO_NUM 14
#define IR_RX_BUF_LEN 2048

AC_data send_data = {
    .head = {0xFF, 0xFF},
    .com1 = 0x22,
    .unk1 = {0, 0, 0, 0, 0, 0},
    .com2 = {0x01, 0x4D, 0x5F},
    .unk2 = 0,
    .in_temp = 0,
    .unk3 = {0, 0, 0, 0, 0, 0, 0, 0, 0},
    .unk4 = 0,
    .unk5 = 0,
    .unk6 = 0,
    .unk7 = 0,
    .unk8 = {0, 0, 0},
};

uint8_t genCRC(AC_data *data)
{
    uint8_t *p = (uint8_t *)data;
    p++;
    p++; // Пропуск шапки.
    data->sum = 0;

    for (uint8_t i = 0; i < sizeof(AC_data) - 3; i++)
        data->sum += *p++; // Получение контрольной суммы.

    return data->sum;
}

void UART_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    uart_param_config(UART_NUM_0, &uart_config);

    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, TX_BUF_SIZE * 2, 0, NULL, 0);
}

void sendData(const void *data, int len)
{
    const int txBytes = uart_write_bytes(UART_NUM_0, data, len);
    ESP_LOGD(TAG_UART, "Wrote %d bytes", txBytes);
    ESP_ERROR_CHECK(uart_wait_tx_done(UART_NUM_0, 100));
}

void prepare_data(void *data, const union HaierAc176Protocol *bits)
{
    //power and health
    send_data.on_off = bits->Power;
    if (bits->Health == 0x01 && send_data.on_off == 0x01)
        send_data.on_off = 0x09; //set health

    switch (bits->Mode)
    {
    case 0x02:
        send_data.mode = 0x04; //dry
        break;
    case 0x04:
        send_data.mode = 0x02; //heat
        break;
    default:
        send_data.mode = bits->Mode;
        break;
    }

    //Enable/Disable Swing
    switch (bits->Swing)
    {
    case 0x0C: //start
        send_data.swing = 1;
        send_data.extra = 0;
        break;
    case 0x00: //stop
        send_data.swing = 0;
        send_data.extra = 0;
        break;
    case 0x03: //to down
        send_data.extra = 0x10;
        send_data.swing = 0;
        break;
    case 0x01: //to up
        send_data.extra = 0x08;
        send_data.swing = 0;
        break;
    default: //reset
        send_data.swing = 0;
        send_data.extra = 0;
        break;
    }

    //Turbo mode
    switch (bits->Turbo)
    {
    case 0x01:
        send_data.extra = 0x02;
        break;
    case 0x02:
        send_data.extra = 0x04;
        break;
    }

    //Set target Temp
    send_data.temp = bits->Temp;

    //Switch FAN speed
    switch (bits->Fan)
    {
    case 0x05: // AUTO
        send_data.fan = 0x03;
        break;
    case 0x03: // Speed 1
        send_data.fan = 0x02;
        break;
    case 0x02: // Speed 2
        send_data.fan = 0x01;
        break;
    case 0x01: // Speed 3
        send_data.fan = 0x00;
        break;
    }

    genCRC(data);
}

void IR_RX_task(void *pvParameter)
{
    ir_rx_data_t ir_data;
    union HaierAc176Protocol data;
    uint8_t size = 0;

    ir_rx_config_t ir_rx_config = {
        .io_num = IR_RX_IO_NUM,
        .buf_len = IR_RX_BUF_LEN};

    ir_rx_init(&ir_rx_config);

    while (1)
    {
        ir_rx_recv_data(&ir_data, 1, portMAX_DELAY);

        if (ir_data.byte == 0xa6)
            size = 0;

        data.raw[size] = ir_data.byte;
        ESP_LOGD(TAG_MAIN, "Data: 0x%x, In integer: %d, Number: %d", ir_data.byte, ir_data.byte, size);
        size++;

        if (size == (sizeof(data) / sizeof(uint8_t)) - 1)
        {
            uint8_t zeros = 00;
            prepare_data(&send_data, &data);

            sendData(&zeros, sizeof(zeros));
            sendData(&send_data, sizeof(AC_data));
            sendData(&zeros, sizeof(zeros));
        }
    }

    vTaskDelete(NULL);
}

void LED_task(void *pvParameter)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << LED_GPIO;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(LED_GPIO, 0);

    while (1)
    {
        if (send_data.on_off)
            gpio_set_level(LED_GPIO, 1);
        else
            gpio_set_level(LED_GPIO, 0);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void app_main()
{
    ESP_LOGI(TAG_MAIN, "Welcome to %s", TAG_MAIN);

    UART_init();

    xTaskCreate(&IR_RX_task, "IR_RX_Task", 2048, NULL, 5, NULL);
    xTaskCreate(&LED_task, "LED_task", 1024, NULL, 5, &xLED_task);
}