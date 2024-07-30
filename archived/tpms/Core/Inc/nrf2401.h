/**
 ******************************************************************************
 * @file    nrf2401.h
 * @brief   TODO: finish this
 *
 */
#ifndef __NRF2401_H__
#define __NRF2401_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "main.h"
#include "spi.h"

// TODO: write documentation

#define RX_BUF_SZ_BYTES 64
#define TX_BUF_SZ_BYTES 64
#define MAX_DATA_BYTES  5

#define PAYLOAD_SZ_BYTES 8

#define SPI_TIMEOUT_MS  200
#define UART_TIMEOUT_MS 500

// --- NRF2401 COMMANDS

#define R_RX_PAYLOAD  (0b01100001)
#define W_TX_PAYLOAD  (0b10100000)
#define R_REGISTER(x) ((uint8_t)x)
#define W_REGISTER(x) ((uint8_t)((1 << 5) | x))
#define FLUSH_TX      (0b11100001)
#define FLUSH_RX      (0b11100010)
#define NOP           (0b11111111)

// --- NRF2401 REGISTERS

// TODO: convert to an enum?
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

// --- STATUS PARSING MACROS

#define STATUS_RX_DR(status) (status >> 6 & 0x1)

// --- FIFO STATUS PARSING MACROS

#define FIFO_STATUS_RX_EMPTY(status) (status >> 0 & 0x1)
#define FIFO_STATUS_RX_FULL(status)  (status >> 1 & 0x1)

// NRF2401 NOTES
// Least significant data byte sent first.
// PRX - primary receiver, PTX - primary transmitter.
// Six NRFs can communication with a single NRF in multiceiver mode.
// Each NRF communication through a different data pipe.
// By default only data pipes 0 and 1 are enabled.
// Each pipe can have a 5 byte configurable address.
// Use the RX_ADDR_Px registers to set the address.
// Use the TX_ADDR register to set the tranmission address.

typedef struct {
  uint8_t command;
  uint8_t data[MAX_DATA_BYTES];
} TxPayload;

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

void pack_payload(TxPayload *payload, uint8_t *buf);
HAL_StatusTypeDef tx_cmd(TxPayload *payload, uint8_t datasz);
HAL_StatusTypeDef tx_rx_cmd(TxPayload *payload, uint8_t txsz, uint8_t *rxbuffer,
                            uint8_t rxsz);
uint8_t nrf2401_get_status(void);
HAL_StatusTypeDef nrf24l01_setup_rf(NrfRfSetup_t *rf_config);

#ifdef __cplusplus
}
#endif

#endif /* __NRF2401_H__ */
