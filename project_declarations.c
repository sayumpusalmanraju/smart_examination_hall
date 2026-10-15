#include "all_macros.h"
#include "lcd.h"
#include "delay.h"
#include "seg.h"
#include "kpm.h"
#include "adc.h"
#include "rtc.h"

/* ----------------------------------------------------------------------
 * Function prototypes
 * -------------------------------------------------------------------- */
u8 password(void);          // Prompts user for password entry, returns match status
void edit_password(void);   // Allows authorized user to change the stored password

/* ----------------------------------------------------------------------
 * External global variables (defined in another source file)
 * save_pass    - stored/saved password value
 * user_pass    - password entered by user for comparison
 * uhour, umin  - exam start hour/minute set by user
 * dur          - exam duration (minutes) set by user
 * flag         - general purpose status flag
 * temppause    - temperature-related pause flag/value
 * tempTime     - temporary copy of duration value
 * pause        - pause state flag for the exam timer
 * -------------------------------------------------------------------- */
extern u32  save_pass, user_pass, uhour, umin, dur, flag, temppause, tempTime;
extern u8 pause;

/* ========================================================================
 * Function: Time
 * Purpose : Menu-driven function to set the RTC (Real-Time Clock) HOUR
 *           and MINUTE values. Runs in a loop until the user selects EXIT.
 * ====================================================================== */
void Time(void)
{
	u8 key;
	s32 temp;

	while(1)
	{
		// Display the Time-setting menu options
		cmdLcd(CLEAR_LCD);
		strLcd("1.HOUR");
		cmdLcd(GOTO_LINE2_POS0);
		strLcd("2.MINUTE");
		cmdLcd(GOTO_LINE3_POS0);
		strLcd("3.EXIT");

		// Wait for a keypad press and act on the selected menu option
		key = Key_Scan();
		switch(key)
		{
			case '1':
				// ---- Set HOUR ----
				cmdLcd(CLEAR_LCD);
				strLcd("Enter Hour :");
				cmdLcd(GOTO_LINE2_POS0);
				temp = ReadNum();          // Read numeric input from keypad

				if(temp == 'c')             // 'c' = user pressed Cancel/Clear
				{
					break;                   // Abort this entry, return to menu
				}

				if(temp < 0 || temp >= 24)  // Validate 24-hour format range
				{
					cmdLcd(CLEAR_LCD);
					strLcd("invalid Hours");
					delay_ms(1000);          // Show error message briefly
				}
				else
				{
					HOUR = temp;             // Commit valid hour to RTC
					SEC  = 0;                // Reset seconds when hour is changed
				}
				break;

			case '2':
				// ---- Set MINUTE ----
				cmdLcd(CLEAR_LCD);
				strLcd("Enter Minutes :");
				cmdLcd(GOTO_LINE2_POS0);
				temp = ReadNum();

				if(temp == 'c')
				{
					break;
				}

				if(temp < 0 || temp >= 60)  // Validate 0-59 minute range
				{
					cmdLcd(CLEAR_LCD);
					strLcd("invalid Minutes");
					delay_ms(1000);
				}
				else
				{
					MIN = temp;              // Commit valid minute to RTC
					SEC = 0;                 // Reset seconds when minute is changed
				}
				break;

			case '3':
				// ---- EXIT ----
				return;                      // Leave the Time-setting menu
		}
	}
}

/* ========================================================================
 * Function: DATE
 * Purpose : Menu-driven function to set the RTC DATE, MONTH, and YEAR.
 *           Loops until the user selects EXIT.
 * ====================================================================== */
void DATE(void)
{
	u8 key;
	s32 temp;

	while(1)
	{
		// Display the Date-setting menu options
		cmdLcd(CLEAR_LCD);
		strLcd("1.DATE");
		cmdLcd(GOTO_LINE2_POS0);
		strLcd("2.MONTH");
		cmdLcd(GOTO_LINE3_POS0);
		strLcd("3.YEAR");
		cmdLcd(GOTO_LINE4_POS0);
		strLcd("4.EXIT");

		key = Key_Scan();
		switch(key)
		{
			case '1':
				// ---- Set DAY OF MONTH ----
				cmdLcd(CLEAR_LCD);
				strLcd("Enter DATE");
				cmdLcd(GOTO_LINE2_POS0);
				temp = ReadNum();

				if(temp == 'c')
				{
					break;
				}

				if(temp < 1 || temp > 31)   // Validate day range (1-31)
				{
					cmdLcd(CLEAR_LCD);
					strLcd("invalid DATE");
					delay_ms(1000);
				}
				else
				{
					DOM = temp;              // Commit valid day-of-month
				}
				break;

			case '2':
				// ---- Set MONTH ----
				cmdLcd(CLEAR_LCD);
				strLcd("Enter MONTH");
				cmdLcd(GOTO_LINE2_POS0);
				temp = ReadNum();

				if(temp == 'c')
				{
					break;
				}

				if(temp < 1 || temp > 12)   // Validate month range (1-12)
				{
					cmdLcd(CLEAR_LCD);
					strLcd("invalid MONTH");
					delay_ms(1000);
				}
				else
				{
					MONTH = temp;            // Commit valid month
				}
				break;

			case '3':
				// ---- Set YEAR ----
				cmdLcd(CLEAR_LCD);
				strLcd("Enter YEAR");
				cmdLcd(GOTO_LINE2_POS0);
				temp = ReadNum();

				if(temp == 'c')
				{
					break;
				}

				if(temp < 1 || temp > 4055) // Validate year range (RTC hardware limit)
				{
					cmdLcd(CLEAR_LCD);
					strLcd("invalid YEAR");
					delay_ms(1000);
				}
				else
				{
					YEAR = temp;             // Commit valid year
				}
				break;

			case '4':
				// ---- EXIT (handled below after switch) ----
				break;
		}

		if(key == '4')
			break;                            // Leave the Date-setting menu
	}
}

/* ========================================================================
 * Function: edit_time
 * Purpose : Top-level menu that lets the user choose between editing
 *           TIME (hour/minute) or DATE (day/month/year), or exiting.
 * ====================================================================== */
void edit_time(void)
{
	u8 key;

	while(1)
	{
		// Display Time/Date edit menu
		cmdLcd(CLEAR_LCD);
		strLcd("1.TIME");
		cmdLcd(GOTO_LINE2_POS0);
		strLcd("2.DATE");
		cmdLcd(GOTO_LINE3_POS0);
		strLcd("3.EXIT");

		key = Key_Scan();
		switch(key)
		{
			case '1':
				Time();                      // Go to Time-setting sub-menu
				break;
			case '2':
				DATE();                      // Go to Date-setting sub-menu
				break;
			case '3':
				return;                      // Exit back to caller (EINT0_RISE)
		}
	}
}

/* ========================================================================
 * Function: start_time
 * Purpose : Menu-driven function to set the EXAM START TIME (hour/minute).
 *           The start time must be equal to or later than the current
 *           RTC time (HOUR/MIN) to be considered valid.
 * ====================================================================== */
void start_time(void)
{
	u8 key;

	while(1)
	{
		// Display exam start-time menu options
		cmdLcd(CLEAR_LCD);
		strLcd("1.HOUR");
		cmdLcd(GOTO_LINE2_POS0);
		strLcd("2.MINUTE");
		cmdLcd(GOTO_LINE3_POS0);
		strLcd("3.EXIT");

		key = Key_Scan();
		switch(key)
		{
			case '1':
				// ---- Set exam start HOUR ----
				cmdLcd(CLEAR_LCD);
				strLcd("Enter Hours:");
				cmdLcd(GOTO_LINE2_POS0);
				uhour = ReadNum();

				if(uhour == 'c')
				{
					break;
				}

				// Start hour must not be earlier than current RTC hour,
				// and must be a valid 24-hour value
				if(uhour < HOUR || uhour > 23)
				{
					cmdLcd(CLEAR_LCD);
					strLcd("Invalid Hour");
					delay_ms(1000);
					uhour = 0;                // Reset invalid entry
				}
				break;

			case '2':
				// ---- Set exam start MINUTE ----
				cmdLcd(CLEAR_LCD);
				strLcd("Enter Minutes:");
				cmdLcd(GOTO_LINE2_POS0);
				umin = ReadNum();

				if(umin == 'c')
				{
					break;
				}

				// If start hour equals current hour, minute must not be
				// earlier than the current RTC minute; also validate range
				if((uhour <= HOUR) && ((umin < MIN) || (umin > 59)))
				{
					cmdLcd(CLEAR_LCD);
					strLcd("Invalid Minutes");
					delay_ms(1000);
					umin = 0;                 // Reset invalid entry
				}
				break;

			case '3':
				// ---- EXIT ----
				return;
		}
	}
}

/* ========================================================================
 * Function: duration
 * Purpose : Menu-driven function to set the exam DURATION in minutes.
 *           Valid range enforced: 5 to 99 minutes.
 * ====================================================================== */
void duration(void)
{
	u8 key;

	while(1)
	{
		// Display duration-setting menu
		cmdLcd(CLEAR_LCD);
		strLcd("1.DURATION");
		cmdLcd(GOTO_LINE2_POS0);
		strLcd("2.EXIT");

		key = Key_Scan();
		switch(key)
		{
			case '1':
				// ---- Set DURATION ----
				cmdLcd(CLEAR_LCD);
				strLcd("Enter duration:");
				cmdLcd(GOTO_LINE2_POS0);
				dur = ReadNum();

				if(dur == 'c')
				{
					break;
				}

				// Valid duration range: minimum 5, maximum 99 minutes
				// (comment in original code says 30, but the check enforces 5)
				if(dur > 99 || dur < 5)
				{
					cmdLcd(CLEAR_LCD);
					strLcd("Invalid duration");
					delay_ms(1000);
					dur = 0;                  // Reset invalid entry
				}
				else
        {
				 tempTime = dur;  //Store duration for later use
				}					
				break;

			case '2':
				// ---- EXIT ----
				return;
		}
	}
}

/* ========================================================================
 * Function: exam_time
 * Purpose : Top-level menu that lets the user choose between setting the
 *           EXAM START TIME or the exam DURATION, or exiting.
 * ====================================================================== */
void exam_time(void)
{
	u8 key;

	while(1)
	{
		// Display Exam Time menu
		cmdLcd(CLEAR_LCD);
		strLcd("1.EXAM TIME");
		cmdLcd(GOTO_LINE2_POS0);
		strLcd("2.DURATION");
		cmdLcd(GOTO_LINE3_POS0);
		strLcd("3.EXIT");

		key = Key_Scan();
		switch(key)
		{
			case '1':
				start_time();                 // Go to exam start-time sub-menu
				break;
			case '2':
				duration();                   // Go to exam duration sub-menu
				break;
			case '3':
				return;                       // Exit back to caller (EINT0_RISE)
		}
	}
}

/* ========================================================================
 * Function: EINT0_RISE
 * Purpose : Interrupt/event handler triggered by an external interrupt
 *           (EINT0) rising edge, typically from a physical button press.
 *           Requires the user to enter a correct password (up to 2
 *           attempts) before granting access to the settings menu, where
 *           they can edit RTC time/date, exam time/duration, or the
 *           password itself.
 * ====================================================================== */
void EINT0_RISE(void)
{
	u8 key, temp;
	s8 count = 2;   // Number of remaining password entry attempts

	/* ------------------------------------------------------------------
	 * Password verification loop
	 * Continues until: password is correct, user cancels, or attempts
	 * run out.
	 * ------------------------------------------------------------------ */
	do
	{
		temp = password();      // Prompt for password, get result status

		if(temp == 'c')          // 'c' = user cancelled password entry
		{
			return;               // Abort and return to caller immediately
		}

		if(temp == 1)             // 1 = password matched
		{
			break;                 // Exit the do-while loop, proceed to menu
		}

		if(temp == 0)              // 0 = password did NOT match
		{
			cmdLcd(CLEAR_LCD);
			count--;                // Decrement remaining attempts
			strLcd("you have ");
			u32Lcd(count);           // Display remaining attempt count
			strLcd(" attempts");
			cmdLcd(GOTO_LINE2_POS0);
			strLcd("wait 3 seconds");
			delay_ms(3000);          // Brief lockout delay between attempts

			if(count == 0)            // No attempts left
			{
				return;                 // Deny access, return to caller
			}
		}
	} while(count);

	/* ------------------------------------------------------------------
	 * Password accepted: show the protected settings menu.
	 * Loops until the user selects EXIT.
	 * ------------------------------------------------------------------ */
	while(1)
	{
		cmdLcd(CLEAR_LCD);
		strLcd("1.EDIT RTC TIME");
		cmdLcd(GOTO_LINE2_POS0);
		strLcd("2.EDIT EXAM TIME");
		cmdLcd(GOTO_LINE3_POS0);
		strLcd("3.EDIT PASSWORD");
		cmdLcd(GOTO_LINE4_POS0);
		strLcd("4.EXIT");

		key = Key_Scan();
		switch(key)
		{
			case '1':
				edit_time();          // Go to RTC time/date editing menu
				break;
			case '2':
				exam_time();          // Go to exam start-time/duration menu
				break;
			case '3':
				edit_password();      // Go to password-change routine
				break;
			case '4':
				break;                // EXIT selected (handled below)
		}

		if(key == '4')
			break;                     // Leave the settings menu
	}
}
