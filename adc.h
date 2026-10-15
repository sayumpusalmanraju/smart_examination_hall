//adc.h
#ifndef ADC_H
#define ADC_H

#include "types.h"

void Init_ADC(void);
void Read_ADC(u32 CHNO, u32* AdcVal, f32* eAR);

#endif // ADC_H
