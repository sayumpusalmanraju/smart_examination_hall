//==================================================================
// Section: LM35 temperature sensor driver (lm35.c)
// The LM35 outputs 10mV per degree Celsius, so temperature (C) is
// simply the sensor voltage (in volts) x 100.
//==================================================================

#include "types.h"
#include "adc.h"
#include "adc_defines.h"
// Reads the LM35 sensor (on ADC channel CH3) and returns the
// temperature in degrees Celsius.
f32 Read_LM35DegC(void)
{
	u32 val;
	f32 eAR;
	Read_ADC(CH3,&val,&eAR);
	return (eAR*100);
}

// Reads the LM35 sensor and returns the temperature converted to
// degrees Fahrenheit.
f32 Read_LM35DegF(void)
{
	 f32 temp;
	temp=Read_LM35DegC();
	return ((temp*1.8)+32);
}

