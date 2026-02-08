#include "Battery_level.h"

void initBatteryLevel(void){
    nrf_saadc_channel_config_t channel0_config = //NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN2);
	{                                                   \
		.resistor_p = NRF_SAADC_RESISTOR_DISABLED,      \
		.resistor_n = NRF_SAADC_RESISTOR_DISABLED,      \
		.gain       = NRF_SAADC_GAIN1_6,                \
		.reference  = NRF_SAADC_REFERENCE_INTERNAL,     \
		.acq_time   = NRF_SAADC_ACQTIME_40US,           \
		.mode       = NRF_SAADC_MODE_SINGLE_ENDED,      \
		.burst      = NRF_SAADC_BURST_ENABLED,         \
		.pin_p      = (nrf_saadc_input_t)(VBAT_APIN),       \
		.pin_n      = NRF_SAADC_INPUT_DISABLED          \
	};
//    nrf_saadc_channel_config_t channel1_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_VDD);

    APP_ERROR_CHECK(nrf_drv_saadc_channel_init(VBAT_SAADC_CH, &channel0_config));
//    APP_ERROR_CHECK(nrf_drv_saadc_channel_init(VDD_SAADC_CH, &channel1_config));
}

uint8_t getBatteryLevel(void){
	const uint8_t samples=10;
	uint32_t sum=0;
	for(uint8_t i=0; i<samples; i++){
		nrf_saadc_value_t value;
		APP_ERROR_CHECK(nrf_drv_saadc_sample_convert(VBAT_SAADC_CH, &value));
		sum+=value>>2;
	}
    return sum/samples;
	
}
uint16_t getBatteryLevelInMiliVolt(void){
	return 3600*getBatteryLevel()/256*2;
}
uint8_t getRegOutVoltage(void){	//todo
    nrf_saadc_value_t value;
    APP_ERROR_CHECK(nrf_drv_saadc_sample_convert(VDD_SAADC_CH, &value));
    return value>>2;
}