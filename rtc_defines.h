//rtc_defines.h
#ifndef RTC_DEFINES_H
#define RTC_DEFINES_H

#define PCLK1         15000000
#define PREINT_VALUE  ((PCLK1/32768)-1)
#define PREFRAC_VALUE (PCLK1-((PREINT_VALUE+1)*32768))

#endif // RTC_DEFINES_H

