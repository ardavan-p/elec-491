/**
 * @file ptn_control.h
 * @author Ardavan Pourkeramati (ardavanpourkeramati@gmail.com)
 * @brief PTN control header file
 * @version 0.1
 * @date 2024-07-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef PTN_CONTROL_H
#define PTN_CONTROL_H

#include <stdbool.h>


typedef enum {
    PTN_STATE_IDLE,
    PTN_STATE_RF_ON,
    PTN_STATE_POLL_,
    S4
} ptn_state_t;

typedef struct {
    ptn_state_t state;
    bool rf_on;
    uint32_t tire_pressure;
    uint32_t tire_temperature;
    ptn_state_t next_state;
} ptn_data_t;

void ptn_state_machine_run(void);

#endif