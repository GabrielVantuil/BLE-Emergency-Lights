#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "app_timer.h"

#include "nrf_drv_gpiote.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_drv_clock.h"
#include "nrf_delay.h"

#include "app_ble.h"
#include "handles.h"
#include "Battery_level.h"

static void log_init(void){
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void lfclk_config(void){
	// initialize the low power low frequency clock
  APP_ERROR_CHECK(nrf_drv_clock_init());

	// request the lf clock to not to generate any events on ticks
	// One tick =  1 value increment in the counter register
  nrf_drv_clock_lfclk_request(NULL);
}




/**@brief Function for application main entry.
 */
int main(void){
    // Initialize.
    lfclk_config();
    log_init();
    timers_init();
    APP_ERROR_CHECK(nrf_pwr_mgmt_init());
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();
//	calcBatteryLevel(NULL);
	// sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, 4);
	
    // Start execution.
	advertising_start();
	
	setLedPwm(1000000, 50);
	
	nrf_gpio_cfg_output(MAIN_LED_CTRL_PIN);
	nrf_gpio_cfg_output(WING1_LED_CTRL_PIN);
	nrf_gpio_cfg_output(WING2_LED_CTRL_PIN);
	nrf_gpio_cfg_output(RGB_COMMON_PIN);
	
	nrf_gpio_cfg_output(LED_R_PIN);

	nrf_gpio_cfg_output(LED_B_PIN);
	
	nrf_gpio_pin_write(RGB_COMMON_PIN, 1);
	nrf_gpio_pin_write(LED_R_PIN, 1);
	nrf_gpio_pin_write(LED_G_PIN, 1);
	nrf_gpio_pin_write(LED_B_PIN, 1);
    // Enter main loop.
    for (;;){
		nrf_gpio_pin_write(LED_G_PIN, !getIsConnected());
		nrf_gpio_pin_write(LED_R_PIN, getIsConnected());
		nrf_gpio_pin_write(RGB_COMMON_PIN, 1);
		nrf_delay_us(1);
		nrf_gpio_pin_write(RGB_COMMON_PIN, 0);
		nrf_delay_us(100);
//        idle_state_handle();
    }
}


/**
 * @}
 */
