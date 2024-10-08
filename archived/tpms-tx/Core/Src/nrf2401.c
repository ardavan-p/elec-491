/**
 ******************************************************************************
 * @file    nrf2401.c
 * @brief   TODO: finish this
 ******************************************************************************
 */

#include "nrf2401.h"

void pack_payload(TxPayload* payload, uint8_t* buf) {
  memcpy((void*)&buf[0], (void*)&payload->command, 1);
  memcpy((void*)&buf[1], (void*)&payload->data, MAX_DATA_BYTES);
}

HAL_StatusTypeDef tx_cmd(TxPayload* payload, uint8_t txsz) {
  HAL_StatusTypeDef status = HAL_ERROR;
  uint8_t txbuffer[TX_BUF_SZ_BYTES] = {0};

  pack_payload(payload, txbuffer);

  HAL_GPIO_WritePin(CHIP_SELECT_GPIO_Port, CHIP_SELECT_Pin, GPIO_PIN_RESET);
  status = HAL_SPI_Transmit(&hspi1, txbuffer, txsz, SPI_TIMEOUT_MS);
  HAL_GPIO_WritePin(CHIP_SELECT_GPIO_Port, CHIP_SELECT_Pin, GPIO_PIN_SET);

  memset(payload, 0, sizeof(*payload));

  return status;
}

HAL_StatusTypeDef tx_rx_cmd(TxPayload* payload, uint8_t txsz, uint8_t* rxbuffer,
                            uint8_t rxsz) {
  HAL_StatusTypeDef status = HAL_ERROR;
  uint8_t txbuffer[TX_BUF_SZ_BYTES] = {0};

  pack_payload(payload, txbuffer);

  HAL_GPIO_WritePin(CHIP_SELECT_GPIO_Port, CHIP_SELECT_Pin, GPIO_PIN_RESET);
  status |= HAL_SPI_Transmit(&hspi1, txbuffer, txsz, SPI_TIMEOUT_MS);
  status |= HAL_SPI_Receive(&hspi1, rxbuffer, rxsz, SPI_TIMEOUT_MS);
  HAL_GPIO_WritePin(CHIP_SELECT_GPIO_Port, CHIP_SELECT_Pin, GPIO_PIN_SET);

  memset(payload, 0, sizeof(*payload));

  return status;
}

uint8_t nrf2401_get_status(void) {
  uint8_t status = 0;
  TxPayload payload = {0};

  payload.command = R_REGISTER(STATUS);

  tx_rx_cmd(&payload, 1, &status, 1);
  return status;
}
