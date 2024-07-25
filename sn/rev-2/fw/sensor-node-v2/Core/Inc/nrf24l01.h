/**
 ******************************************************************************
 * @file    nrf24l01.h
 * @brief   Contains definitions and declarations for nRF24L01 driver.
 *
 */
#ifndef __NRF2401_H__
#define __NRF2401_H__

#include "main.h"
#include "stdint.h"
#include "stm32u0xx_hal_def.h"

// TODO: add general documentation here.
// Least significant data byte sent first.
// PRX - primary receiver, PTX - primary transmitter.
// Six NRFs can communication with a single NRF in multiceiver mode.
// Each NRF communication through a different data pipe.
// By default only data pipes 0 and 1 are enabled.
// Each pipe can have a 5 byte configurable address.
// Use the RX_ADDR_Px registers to set the address.
// Use the TX_ADDR register to set the tranmission address.

#ifdef __cplusplus
extern "C" {
#endif

// --- USER CONFIGURATION ---

// chip select port and pin configuration
#define CS_GPIO_PORT CHIP_SELECT_Pin
#define CS_GPIO_PIN  CHIP_SELECT_GPIO_Port

#define RX_BUF_SZ_BYTES 64
#define TX_BUF_SZ_BYTES 64

// maximum number of bytes allowed in a single SPI transaction
#define MAX_DATA_BYTES 32

// size of payload sent wirelessly
#define PAYLOAD_SZ_BYTES 8

#define SPI_TIMEOUT_MS  200
#define UART_TIMEOUT_MS 500

// --- NRF24L01 COMMANDS ---

#define R_RX_PAYLOAD       (0b01100001)
#define W_TX_PAYLOAD       (0b10100000)
#define R_REGISTER(reg)    ((uint8_t)reg)
#define W_REGISTER(reg)    ((uint8_t)((1 << 5) | reg))
#define FLUSH_TX           (0b11100001)
#define FLUSH_RX           (0b11100010)
#define W_TX_PAYLOAD_NOACK (0b10110000)
#define NOP                (0b11111111)

// --- NRF24L01 REGISTERS ---

#define CONFIG_REGISTER 0x00
#define EN_RXADDR       0x02
#define SETUP_RETR      0x04
#define RF_CH           0x05
#define RF_SETUP        0x06
#define STATUS          0x07
#define OBSERVE_TX      0x08
#define RPD             0x09

#define RX_ADDR_P0 0x0A
#define RX_ADDR_P1 0x0B
#define RX_ADDR_P2 0x0C
#define RX_ADDR_P3 0x0D
#define RX_ADDR_P4 0x0E
#define RX_ADDR_P5 0x0F
#define TX_ADDR    0x10

#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16

#define FIFO_STATUS 0x17
#define DYN_PD      0x1c
#define FEATURE     0x1d

// --- STATUS PARSING MACROS ---

#define STATUS_RX_DR(status)  (status >> 6 & 0x1)
#define STATUS_TX_DS(status)  (status >> 5 & 0x1)
#define STATUS_MAX_RT(status) (status >> 4 & 0x1)

// --- TYPE DECLARATIONS ---

// TODO: potentially use this later
typedef struct {
  uint16_t cs_port;
  uint16_t cs_pin;
  uint16_t ce_port;
  uint16_t ce_pin;
} NrfConfig;

typedef enum {
  ONE_MBPS = 0x0,
  TWO_MBPS = 0x1,
  TWO_FIFTY_KBPS = 0x2,
} NrfDataRate_e;

typedef enum {
  NEG_EIGHTEEN_DBM = 0x0,
  NEG_TWELVE_DBM = 0x1,
  NEG_SIX_DBM = 0x2,
  ZERO_DBM = 0x3,
} NrfTransmitPower_e;

typedef struct {
  NrfDataRate_e data_rate;
  NrfTransmitPower_e data_power;
} NrfRfSetup_t;

// --- FUNCTION DECLARATIONS ---

/**
 * Sends an SPI command to the nRF24L01. This command ignores the returned data
 * from the peripheral.
 *
 * NOTE: this function zeroes out the [0:txsz-1] range in the `tx_payload`
 * buffer after transmission is complete.
 *
 * @param command - command word for desired SPI command.
 * @param tx_payload - pointer to the payload buffer for additional SPI data (if
 * required). Can be `NULL`.
 * @param txsz - number of bytes from the payload buffer to transmit. Make sure
 * that txsz <= length of `tx_payload`.
 * @returns HAL_StatusTypeDef - status enum indicating if the SPI transaction
 * was successful.
 */
HAL_StatusTypeDef tx_spi_cmd(uint8_t command, uint8_t* tx_payload,
                             uint8_t txsz);
/**
 * @brief Sends an SPI command to the nRF24L01 and receives the returned data
 * from the peripheral.
 *
 * NOTE: this function zeroes out the [0:txsz-1] range in the `tx_payload`
 * buffer after transmission is complete.
 *
 * @param[in] command - command word for desired SPI command.
 * @param[in] tx_payload - pointer to the payload buffer for additional SPI data
 * if required. Can be `NULL`.
 * @param[in] txsz - number of bytes from the payload buffer to transmit. Make
 * sure that txsz <= length of `tx_payload`. Can be 0.
 * @param[out] rx_buffer - pointer to the buffer to store
 * @param[in] rxsz - max amount of data in bytes to read back into the buffer
 * pointed to by `rx_buffer`.
 * @returns HAL_StatusTypeDef - status enum indicating if the SPI transaction
 * was successful.
 */
HAL_StatusTypeDef tx_rx_spi_cmd(uint8_t command, uint8_t* tx_payload,
                                uint8_t txsz, uint8_t* rx_buffer, uint8_t rxsz);

/**
 * @brief Reads the nRF24L01 STATUS register. This register can be parsed with
 * the status parsing macros.
 *
 * @returns - contents of the nRF24L01 STATUS register.
 */
uint8_t nrf24l01_get_status(void);

/**
 * @brief Sends an RF message using the nRF24L01.
 *
 * @param[in] buf - pointer to the buffer containing the message to transmit
 * over RF.
 * @param[in] bufsz - number of bytes from the buffer to transmit over RF.
 * @returns - contents of the nRF24L01 STATUS register.
 */
HAL_StatusTypeDef nrf24l01_send_msg(uint8_t* buf, uint8_t bufsz);

/**
 * @brief Sends an RF message using the nRF24L01 and disables
 * auto-acknowledgement for that particular message.
 *
 * WARNING: before using this function, make sure that the `W_TX_PAYLOAD_NOACK`
 * command has been enabled by setting bit 0 of the `FEATURE` register.
 *
 * @param[in] buf - pointer to the buffer containing the message to transmit
 * over RF.
 * @param[in] bufsz - number of bytes from the buffer to transmit over RF.
 * @returns - contents of the nRF24L01 STATUS register.
 */
HAL_StatusTypeDef nrf24l01_send_msg_noack(uint8_t* buf, uint8_t bufsz);

/**
 * @brief Configures the RF communication characteristics (specifically transmit
 * power and air data rate) of the nRF24L01.
 *
 * @param[in] rf_config - pointer to a setup struct that stores the desired
 * configuration.
 * @returns - status indicating whether the function succeeded or not.
 */
HAL_StatusTypeDef nrf24l01_setup_rf(NrfRfSetup_t* rf_config);

#ifdef __cplusplus
}
#endif

#endif /* __NRF2401_H__ */
