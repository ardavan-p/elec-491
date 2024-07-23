/**
 ******************************************************************************
 * @file    nrf24l01.c
 * @brief   TODO: finish this
 ******************************************************************************
 */

#include "nrf24l01.h"

#include "main.h"
#include "spi.h"

#define CHIP_SELECT_GPIO_Port NRF_CSN_GPIO_Port
#define CHIP_SELECT_Pin NRF_CSN_Pin

#define CHIP_ENABLE_GPIO_Port NRF_CE_GPIO_Port
#define CHIP_ENABLE_Pin NRF_CE_Pin

HAL_StatusTypeDef tx_spi_cmd(uint8_t command, uint8_t *tx_payload,
                             uint8_t txsz) {
  HAL_StatusTypeDef status = HAL_ERROR;
  uint8_t txbuffer[TX_BUF_SZ_BYTES] = {0};

  if (txsz > MAX_DATA_BYTES) {
    return HAL_ERROR;
  }

  txbuffer[0] = command;
  memcpy(&txbuffer[1], tx_payload, txsz);

  HAL_GPIO_WritePin(CHIP_SELECT_GPIO_Port, CHIP_SELECT_Pin, GPIO_PIN_RESET);
  status = HAL_SPI_Transmit(&hspi1, txbuffer, txsz + 1, SPI_TIMEOUT_MS);
  HAL_GPIO_WritePin(CHIP_SELECT_GPIO_Port, CHIP_SELECT_Pin, GPIO_PIN_SET);

  memset(tx_payload, 0, txsz);

  return status;
}

HAL_StatusTypeDef tx_rx_spi_cmd(uint8_t command, uint8_t *tx_payload,
                                uint8_t txsz, uint8_t *rx_buffer,
                                uint8_t rxsz) {
  HAL_StatusTypeDef status = HAL_ERROR;
  uint8_t tx_buffer[TX_BUF_SZ_BYTES] = {0};

  if (txsz > MAX_DATA_BYTES) {
    return HAL_ERROR;
  }

  tx_buffer[0] = command;

  if (tx_payload != NULL) {
    memcpy(&tx_buffer[1], tx_payload, txsz);
  }

  HAL_GPIO_WritePin(CHIP_SELECT_GPIO_Port, CHIP_SELECT_Pin, GPIO_PIN_RESET);
  status |= HAL_SPI_Transmit(&hspi1, tx_buffer, txsz + 1, SPI_TIMEOUT_MS);
  status |= HAL_SPI_Receive(&hspi1, rx_buffer, rxsz, SPI_TIMEOUT_MS);
  HAL_GPIO_WritePin(CHIP_SELECT_GPIO_Port, CHIP_SELECT_Pin, GPIO_PIN_SET);

  memset(tx_payload, 0, txsz);

  return status;
}

uint8_t nrf24l01_get_status(void) {
  uint8_t status = 0;
  tx_rx_spi_cmd(R_REGISTER(STATUS), NULL, 0, &status, 1);
  return status;
}

HAL_StatusTypeDef nrf24l01_send_msg(uint8_t *buf, uint8_t bufsz) {
  HAL_StatusTypeDef status = HAL_ERROR;
  uint8_t rf_payload[PAYLOAD_SZ_BYTES] = {0};

  if (bufsz != PAYLOAD_SZ_BYTES) {
    return HAL_ERROR;
  }

  memcpy(rf_payload, buf, PAYLOAD_SZ_BYTES);

  // enable the chip
  HAL_GPIO_WritePin(CHIP_ENABLE_GPIO_Port, CHIP_ENABLE_Pin, GPIO_PIN_SET);

  // place message in TX FIFO
  status = tx_spi_cmd(W_TX_PAYLOAD, rf_payload, PAYLOAD_SZ_BYTES);

  // disable the chip
  HAL_GPIO_WritePin(CHIP_ENABLE_GPIO_Port, CHIP_ENABLE_Pin, GPIO_PIN_RESET);

  return status;
}

HAL_StatusTypeDef nrf24l01_send_msg_noack(uint8_t *buf, uint8_t bufsz) {
  HAL_StatusTypeDef status = HAL_ERROR;
  uint8_t rf_payload[PAYLOAD_SZ_BYTES] = {0};

  if (bufsz != PAYLOAD_SZ_BYTES) {
    return HAL_ERROR;
  }

  memcpy(rf_payload, buf, PAYLOAD_SZ_BYTES);

  HAL_GPIO_WritePin(CHIP_ENABLE_GPIO_Port, CHIP_ENABLE_Pin, GPIO_PIN_SET);

  // place message in TX FIFO
  status = tx_spi_cmd(W_TX_PAYLOAD_NOACK, rf_payload, PAYLOAD_SZ_BYTES);

  HAL_GPIO_WritePin(CHIP_ENABLE_GPIO_Port, CHIP_ENABLE_Pin, GPIO_PIN_RESET);

  return status;
}

HAL_StatusTypeDef nrf24l01_setup_rf(NrfRfSetup_t *rf_config) {
  HAL_StatusTypeDef status = HAL_ERROR;
  uint8_t tx_payload[TX_BUF_SZ_BYTES] = {0};
  uint8_t rf_setup = 0;

  uint8_t rf_dr_lo = (rf_config->data_rate >> 1) & 0x1;
  uint8_t rf_dr_hi = (rf_config->data_rate >> 0) & 0x1;
  uint8_t rf_pwr = rf_config->data_power << 1;

  // RF_DR_LOW [5] R:W
  rf_setup = rf_setup | (rf_dr_lo << 5);
  // RF_DR_HIGH [3] R:W
  rf_setup = rf_setup | (rf_dr_hi << 3);
  // RF_PWR [2:1] R:W
  rf_setup = rf_setup | (rf_pwr);

  tx_payload[0] = rf_setup;
  status = tx_spi_cmd(W_REGISTER(RF_SETUP), tx_payload, 1);

  return status;
}
