//we provide some default value for rtc time and date before editing its values
#include <LPC21xx.h>
#include "rtc_defines.h"
#include "lcd.h"
void Default_Rtc_time(void)
{
	CCR=1<<1; //reset 
	PREINT=PREINT_VALUE;
	PREFRAC=PREFRAC_VALUE;
	CCR=1<<0;//ctc is enable
	SEC=40;
	MIN=59;
	HOUR=12;
	DOM=12;
	MONTH=6;
	YEAR=2026;
}

//this function is used for displaying current time 
void Rtc_Time_Display(void)
{
	//HH:MM:SS
		charLcd(HOUR/10+'0');
		charLcd(HOUR%10+'0');
		charLcd(':');
		charLcd(MIN/10+'0');
		charLcd(MIN%10+'0');
		charLcd(':');
		charLcd(SEC/10+'0');
		charLcd(SEC%10+'0');
		strLcd("            ");
}

//this function is used for displaying date 
void Rtc_Date_Display(void)
{
   	//day/month/year
		charLcd(DOM/10+'0');
		charLcd(DOM%10+'0');
		charLcd('/');
		charLcd(MONTH/10+'0');
		charLcd(MONTH%10+'0');
		charLcd('/');
		u32Lcd(YEAR);
		strLcd("          ");
}

