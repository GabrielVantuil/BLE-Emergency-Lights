#include "app.h"
#define PWM_CLK_FREQ 125000


bool ledPowerLocked;
static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);
static nrf_pwm_values_individual_t seq_values;

static nrf_pwm_sequence_t const seq = {
    .values.p_individual = &seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

void gpio_init(void){
    APP_ERROR_CHECK(nrf_drv_saadc_init(NULL, NULL));
	initBatteryLevel();
	setLedPwm(1000000, 50);
	
	nrf_gpio_cfg_output(MAIN_LED_CTRL_PIN);
	nrf_gpio_cfg_output(WING1_LED_CTRL_PIN);
	nrf_gpio_cfg_output(WING2_LED_CTRL_PIN);
	nrf_gpio_cfg_output(RGB_COMMON_PIN);
	
	nrf_gpio_cfg_output(LED_R_PIN);
	nrf_gpio_cfg_output(LED_G_PIN);
	nrf_gpio_cfg_output(LED_B_PIN);
	
	nrf_gpio_pin_write(RGB_COMMON_PIN, 1);
	nrf_gpio_pin_write(LED_R_PIN, 1);
	nrf_gpio_pin_write(LED_G_PIN, 1);
	nrf_gpio_pin_write(LED_B_PIN, 1);
	
    nrf_saadc_channel_config_t adc_config =	{			\
		.resistor_p = NRF_SAADC_RESISTOR_DISABLED,      \
		.resistor_n = NRF_SAADC_RESISTOR_DISABLED,      \
		.gain       = SAADC_CH_CONFIG_GAIN_Gain1_6,                \
		.reference  = NRF_SAADC_REFERENCE_INTERNAL,     \
		.acq_time   = NRF_SAADC_ACQTIME_40US,           \
		.mode       = NRF_SAADC_MODE_SINGLE_ENDED,      \
		.burst      = NRF_SAADC_BURST_ENABLED,         	\
		.pin_p      = (nrf_saadc_input_t)(LED_SHUNT_APIN),       \
		.pin_n      = NRF_SAADC_INPUT_DISABLED          \
	};
	
    APP_ERROR_CHECK(nrf_drv_saadc_channel_init(LED_SHUNT_SAADC_CH, &adc_config));
}
void setLed(bool val){
	if(val)	setLedPwm(1000000, 100);
	else	setLedPwm(1000000, 0);
}

void setLedOff(void * p_context){
	if(ledPowerLocked) return;
	setLed(LED_OFF);
}

void checkBat(void * p_context){
	static int lowBatCount=0;
	uint16_t vbat = getBatteryLevelInMiliVolt();
	if(vbat <= 3400) {
		lowBatCount++;
	}
	else lowBatCount=0;
	NRF_LOG_INFO("%dmv, count %d", vbat, lowBatCount);
	if(lowBatCount > 6){
		setLedPwm(1000000, 0);
		lowBatCount=0;
		return;
	}
}

void setLedPwm(uint32_t mHertz, uint16_t duty){	
	uint32_t top_value;
	if((PWM_CLK_FREQ/(mHertz/1000)) >= 65535) top_value = 65535;	// "top_value" is a uint16_t, max value = 65536, 
	else top_value = PWM_CLK_FREQ/(mHertz/1000);	//for clk=125kHz, 125 = 1000kHz, so max "top_value" = 1.907Hz, 
													//which is the lowest frequency allowed for 125kHz
	
    nrfx_pwm_uninit(&m_pwm0);
	nrfx_pwm_config_t const config0 = {
		.output_pins = {
			MAIN_LED_CTRL_PIN,
			WING1_LED_CTRL_PIN,
			WING2_LED_CTRL_PIN,
//			NRFX_PWM_PIN_NOT_USED,
//			NRFX_PWM_PIN_NOT_USED,
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

void leds_init(void){
	setLedPwm(10000000, 70);
}

uint8_t getLedCurrent(void){
	const uint8_t samples=10;
	uint32_t sum=0;
	for(uint8_t i=0; i<samples; i++){
		nrf_saadc_value_t value;
		APP_ERROR_CHECK(nrf_drv_saadc_sample_convert(LED_SHUNT_SAADC_CH, &value));
		sum+=value>>2;
	}
    return sum/samples;
}


