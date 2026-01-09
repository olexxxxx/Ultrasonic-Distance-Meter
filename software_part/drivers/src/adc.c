#include "adc.h"



volatile Button_ID current_button = BTN_NONE;
System_State current_state = STATE_MEASURE;
uint16_t threshold_val = 0;
uint8_t units = 1; //cm = 1, inches = 0
uint8_t button_pressed_flag = 0;

void pd2_adc_init(void){
// Set up ADC
	ADC1->CSR |= 	ADC1_CHANNEL_3; // Select channel to read
	ADC1->CR2 = 	ADC1_CR2_ALIGN; // Place LSB in lower register
	ADC1->CR1 = 	ADC1_CR1_ADON; // Power on the ADC
}

void adc_start_conversion(void){
	ADC1->CR1 |= ADC1_CR1_ADON;	
}

unsigned long adc_read(void){
	unsigned long result;
	// Clear done flag
	ADC1->CSR &= ~ADC1_CSR_EOC;
	// Load ADC reading (least-significant byte must be read first)
	result = ADC1->DRL;
	result |= (ADC1->DRH << 8);
	return result;
}
uint16_t convert_treshold(uint8_t unit, uint16_t threshold){
	if(unit == 1){
		return threshold;
	}
	return (uint16_t)threshold/2.54;
}

void show_threshold(uint16_t threshold){
    char buf[5];
    sprintf(buf, "%4d", threshold);
    tm_display_digits(buf);

}