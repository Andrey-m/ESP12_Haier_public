#ifndef _IR_TASK_H_
#define _IR_TASK_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define IR_RX_BIT_NUM 8
#define IR_RX_DATA0_US 500
#define IR_RX_DATA1_US 1600
#define IR_RX_ERROR_US 70 // Used to compensate errors

  /**
 * @brief ir rx initialization parameter structure type definition
 */
  typedef struct
  {
    uint32_t io_num;
    uint32_t buf_len;
  } ir_rx_config_t;

  /**
 * @brief ir rx nec data union type definition
 */
  typedef union
  {
    uint8_t byte; /*!< union fill */
  } ir_rx_data_t;

  /**
  * @brief Disable the ir rx
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_FAIL ir rx has not been initialized yet
  */
  esp_err_t ir_rx_disable();

  /**
  * @brief Enable the ir rx
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_FAIL ir rx has not been initialized yet
  */
  esp_err_t ir_rx_enable();

  /**
 * @brief Receive infrared data
 *
 * @param data Pointer to the rx data buffer
 * @param len Length of ir_rx_data, range: 0 < len < (uint16_t)
 * @param timeout_ticks freertos timeout ticks
 * 
 * @return
 *     - -1 error
 *     - length The actual length of data received
 */
  int ir_rx_recv_data(ir_rx_data_t *data, size_t len, uint32_t timeout_ticks);

  /**
  * @brief Deinit the ir rx
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_FAIL ir rx has not been initialized yet
  */
  esp_err_t ir_rx_deinit();

  /**
  * @brief Initialize the ir rx
  *
  * @param config Pointer to deliver initialize configuration parameter
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_NO_MEM malloc fail
  *     - ESP_FAIL ir rx has been initialized
  */
  esp_err_t ir_rx_init(ir_rx_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* _IR_TASK_H_ */