#include "handles.h"
#include "utils.h"


extern bool ledPowerLocked;
void timers_init(void){
    // Initialize timer module, making it use the scheduler
    APP_ERROR_CHECK(app_timer_init());
	APP_ERROR_CHECK(app_timer_create(&power_off_led_timer_id, APP_TIMER_MODE_SINGLE_SHOT, setLedOff));
	APP_ERROR_CHECK(app_timer_create(&check_bat_timer_id, APP_TIMER_MODE_REPEATED, checkBat));
	APP_ERROR_CHECK(app_timer_create(&update_info_timer_id, APP_TIMER_MODE_REPEATED, updateInfoPayload));
	
	APP_ERROR_CHECK(app_timer_start(check_bat_timer_id, APP_TIMER_TICKS(10000), NULL));
	APP_ERROR_CHECK(app_timer_start(update_info_timer_id, APP_TIMER_TICKS(1000), NULL));
}

void connectionTimeout(void * p_context){
	sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void idle_state_handle(void){
    if (NRF_LOG_PROCESS() == false){
        nrf_pwr_mgmt_run();
    }
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
void conn_params_error_handler(uint32_t nrf_error){
    APP_ERROR_HANDLER(nrf_error);
}
/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name){
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

void lock_handler           (uint16_t conn_handle, ble_torch_s_t * p_torch_s, uint8_t lock){
	ledPowerLocked = lock;
}

void led_power_handler      (uint16_t conn_handle, ble_torch_s_t * p_torch_s, const uint8_t *params){
	if(ledPowerLocked) return;
	APP_ERROR_CHECK(app_timer_stop(power_off_led_timer_id));
    setLed(params[0]?LED_ON : LED_OFF);
	uint32_t timeout;
	ArrayToInt32(params, 1, timeout);
	if(timeout)	APP_ERROR_CHECK(app_timer_start(power_off_led_timer_id, APP_TIMER_TICKS(timeout), NULL));
}


void led_pwm_handler       	(uint16_t conn_handle, ble_torch_s_t * p_torch_s, const uint8_t *params){
	if(ledPowerLocked) return;
	uint32_t freq, duty, timeout;
	ArrayToInt32(params, 0, freq);
	duty = (params[4]<<8) + params[5];
	ArrayToInt32(params, 6, timeout);
	if(timeout)	APP_ERROR_CHECK(app_timer_start(power_off_led_timer_id, APP_TIMER_TICKS(timeout), NULL));
	setLedPwm(freq, duty);
}
