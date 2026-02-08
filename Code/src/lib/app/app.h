#ifndef APP_H__
#define APP_H__
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"
#include "nrf_gpio.h"
#include "board_config.h"
#include "nrf_drv_saadc.h"
#include "Battery_level.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

void gpio_init(void);

void leds_init(void);
void setLed(bool val);
void blinkLed(uint8_t led);

void setLedOff(void * p_context);
void checkBat(void * p_context);
void setLedPwm(uint32_t mHertz, uint16_t duty);

void blinkLedHandler(void * p_context);
uint8_t getLedCurrent(void);

#endif
