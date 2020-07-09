#include "HXM8_MCP2515_COMMUNICATION.h"
#include "MCP2515_can_controller.h"


/***begin FreeRTOS libraries***/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
/***end FreeRTOS libraries***/

/***begin user configured libraries***/
#include "driver/gpio.h"
#include "esp_log.h"
#include "spi_master_HAL.h"
#include "tcp_client_station.h"
#include <stdlib.h>
/***end user configured libraries***/

/***begin function definitions***/
static void rx_0_isr_handler(void *arg);
static void can_comm_task_rx(void* pvParameters);
/***end function definitions***/

/***begin global variables***/
static const char* TAG = "HXM8_MCP2515_COMM";

static uint32_t* rx_0_identifier;
static uint32_t* rx_0_data;
static uint32_t* rx_0_id_data;

static uint32_t* rx_1_identifier;
static uint32_t* rx_1_data;
static uint32_t* rx_1_id_data;

static QueueHandle_t rx_buff_filled;
static TaskHandle_t can_comm_task_1 = NULL;
/***end global variables***/


/***begin ISR handler***/
static void rx_0_isr_handler(void *arg) {
    mcp2515_rx_buffer_read(0, rx_0_identifier, rx_0_data);
    *rx_0_id_data = *rx_0_identifier;
    *(rx_0_id_data + 1) = *(rx_0_data);
    *(rx_0_id_data + 2) = *(rx_0_data + 1);
    
    xQueueSendFromISR(rx_buff_filled, &rx_0_id_data, NULL);
}
/***end ISR handler***/

/***begin CAN communication task***/
static void can_comm_task_rx(void* pvParameters) {
    BaseType_t xStatus;
    uint32_t* data;
    data = (uint32_t*)malloc(3 * sizeof(uint32_t));

    ESP_LOGI(TAG, "CAN communication online");

    while(1) {
        xStatus = xQueueReceive(rx_buff_filled, &data, portMAX_DELAY);  //wait for items to be available in the queue
        if(xStatus == pdPASS) {
            /***begin functions for queue handling***/
            ESP_LOGI(TAG, "RX BUFFER DATA: %d + %d RX BUFFER ID: %d", *(data + 1), *(data + 2), *data);//, data_to_send);
            tcp_tx_data(data);
            vTaskDelay(15 / portTICK_PERIOD_MS);
            tcp_tx_data(data + 1);
            vTaskDelay(15 / portTICK_PERIOD_MS);
            tcp_tx_data(data + 2);
            vTaskDelay(15 / portTICK_PERIOD_MS);
            /***end functions for queue handling***/
        }
        else {
            continue;
        }
    }
    free(data);
}
/***end CAN communication task***/


void hxm8_mcp2515_init(void) {
    rx_0_data = (uint32_t*)malloc(2 * sizeof(uint32_t));
    rx_0_identifier = (uint32_t*)malloc(sizeof(uint32_t));
    rx_0_id_data = (uint32_t*)malloc(3 * sizeof(uint32_t));

    rx_1_data = (uint32_t*)malloc(2 * sizeof(uint32_t));
    rx_1_identifier = (uint32_t*)malloc(sizeof(uint32_t));
    rx_1_id_data = (uint32_t*)malloc(3 * sizeof(uint32_t));

    /***begin SPI initialization***/
    spi_interface_init();
    /***end SPI initialization***/

    /***begin initialize mcp2515 driver***/
    mcp2515_init();
    /***end initialize mcp2515 driver***/
    
    /***begin GPIO configuration***/
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.mode = GPIO_MODE_INPUT; 
    io_conf.pull_up_en = 1;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    gpio_config(&io_conf);
    /***end GPIO configuration***/

    /***begin queue handle gpio event from ISR***/
    rx_buff_filled = xQueueCreate(5, sizeof(uint32_t));
    /***end queue handle gpio event from ISR***/

    /***begin accessory tasks***/
    wifi_init();
    tcp_stack_start();
    /***end accessory tasks***/

    /***begin CAN communication task***/
    xTaskCreate(can_comm_task_rx, "can_comm_task_rx", 2048, NULL, CAN_BUS_TASK_PRIORITY, &can_comm_task_1);
    /***end CAN communication task***/

    /***begin configure and initialize interrupts for the system***/
    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, rx_0_isr_handler, (void *) 0);
    //hook isr handler for specific gpio pin
    //gpio_isr_handler_add(GPIO_INPUT_IO_1, isr_handler, (void *) 1);
    /***end configure and initialize interrupts for the system***/
}