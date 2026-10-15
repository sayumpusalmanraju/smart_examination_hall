//==================================================================
// Section: ADC driver (adc.c)
// Configures and reads the on-chip Analog-to-Digital Converter.
//==================================================================

// Configures the ADC pin function select and powers up/enables the
// ADC with the configured clock divider.
#include <LPC21xx.h>
#include "adc_defines.h"
#include "types.h"
#include "delay.h"
void Init_ADC(void)
{
	PINSEL1=0x10000000;
	ADCR=(PDN_BIT|CLKDIV_VALUE);
}

// Performs a single ADC conversion on channel 'CHN0'.
// Outputs: *AdcVal = raw 10-bit ADC result (0-1023)
//          *eAR    = corresponding voltage in volts (0 - 3.3V)
void Read_ADC(u32 CHN0,u32* AdcVal,f32* eAR)
{
	ADCR&=~(255<<0);            // clear previous channel selection
	ADCR|=(CHN0|START_CONV);    // select channel and start conversion
	delay_us(3);
	while(((ADDR>>DONE_BIT)&1)==0);  // wait for conversion to complete
	ADCR&=~(START_CONV);
	*AdcVal=((ADDR>>RESULT)&1023);
	*eAR=(3.3/1024)*(*AdcVal);   // convert raw code to voltage
		
}
