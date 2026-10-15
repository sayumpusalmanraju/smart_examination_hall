#include "types.h"
// Busy-wait delay of approximately 'i' microseconds
void delay_us(u32 i)
{
	 i*=12;
	while(i--);
}

// Busy-wait delay of approximately 'i' milliseconds
void delay_ms(u32 i)
{
	 i*=12000;
	while(i--);
}

// Busy-wait delay of approximately 'i' seconds
void delay_s(u32 i)
{
	 i*=12000000;
	while(i--);
}

