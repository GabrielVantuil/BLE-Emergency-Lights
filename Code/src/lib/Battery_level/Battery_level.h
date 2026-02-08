#ifndef BATTERY_LEVEL_H__
#define BATTERY_LEVEL_H__

#include "nrf_drv_saadc.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "board_config.h"

#define SAMPLES_IN_BUFFER 	1

static nrf_saadc_value_t     m_buffer_pool[SAMPLES_IN_BUFFER];

void initBatteryLevel(void);
uint8_t getBatteryLevel(void);
uint16_t getBatteryLevelInMiliVolt(void);

#endif
