#include "handles.h"
#include "utils.h"

#define PWM_CLK_FREQ 125000
static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);
static nrf_pwm_values_individual_t seq_values;

static nrf_pwm_sequence_t const seq = {
    .values.p_individual = &seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

bool ledPowerLocked;

void timers_init(void){
    // Initialize timer module, making it use the scheduler
    APP_ERROR_CHECK(app_timer_init());
	APP_ERROR_CHECK(app_timer_create(&power_off_led_timer_id, APP_TIMER_MODE_SINGLE_SHOT, setLedOff));
}

void setLed(bool val){
	if(val)	setLedPwm(1000000, 100);
	else	setLedPwm(1000000, 0);
}

void setLedOff(void * p_context){
	if(ledPowerLocked) return;
	setLed(LED_OFF);
}

void setLedPwm(uint32_t mHertz, uint16_t duty){	
	uint32_t top_value;
	if((PWM_CLK_FREQ/(mHertz/1000)) >= 65535) top_value = 65535;	// "top_value" is a uint16_t, max value = 65536, 
	else top_value = PWM_CLK_FREQ/(mHertz/1000);	//for 125kHz, 125 = 1000kHz
	
    nrfx_pwm_uninit(&m_pwm0);
	nrfx_pwm_config_t const config0 = {
		.output_pins = {
			MAIN_LED_CTRL_PIN,
			//WING1_LED_CTRL_PIN,
			//WING2_LED_CTRL_PIN,
			NRFX_PWM_PIN_NOT_USED,
			NRFX_PWM_PIN_NOT_USED,
			NRFX_PWM_PIN_NOT_USED  // channel 3
		},
		.irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
		.base_clock   = NRF_PWM_CLK_125kHz,
		.count_mode   = NRF_PWM_MODE_UP,
		.top_value    = top_value,                  // Period = (1000Hz)
		.load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
		.step_mode    = NRF_PWM_STEP_AUTO
	};
		
    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm0, &config0, NULL));
	
    seq_values.channel_0 = top_value*(100-duty)/100;
    seq_values.channel_1 = seq_values.channel_0;
    seq_values.channel_2 = seq_values.channel_0;

    nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_LOOP);
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
void connectionTimeout(void * p_context){
	sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
}

/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by the application.
 */
void leds_init(void){
	setLedPwm(10000000, 70);
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
