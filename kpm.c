//==================================================================
// Section: 4x4 matrix keypad driver (kpm.c)
// Scans a 4x4 matrix keypad by driving one row low at a time and
// reading the column lines to detect which key is pressed.
//==================================================================

#include <LPC21xx.h>
#include "types.h"
#include "kpm_defines.h"
#include "lcd.h"
#include "delay.h"
//u32 KpmLut[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};

// Maps [row][col] scan coordinates to the corresponding keypad
// character/symbol.
u8 KpmLut[4][4]={{'7','8','9','/'},{'4','5','6','*'},{'1','2','3','-'},{'c','0','=','+'}};

// Configures the row output pins as GPIO outputs.
void Kpm_init(void)
{
   IODIR1|=15<<ROW0;
}

// Quick check of whether any key is currently pressed on the
// currently-driven row. Returns 0 if at least one column line is
// pulled low (key pressed), otherwise returns 1 (no key pressed).
u32 colscan(void)
{
   if(((IOPIN1>>COL0)&15)<15)
   {
     return 0;
         }
         return 1;
}

// Drives each row low in turn until a pressed key pulls a column
// line low, and returns the index (0-3) of that row.
u32 Row_Check(void)
{
   int rno;
   for(rno=0;rno<4;rno++)
   {
    IOPIN1=(IOPIN1&~(0xFF<<ROW0))|((~(1<<rno))<<ROW0);
		 if(colscan()==0)
            {
                  break;
                }
        }
        IOCLR1=15<<ROW0;   // release all rows
        return rno;
}

// Reads the column lines and returns the index (0-3) of the column
// that is pulled low (the pressed key's column).
 u32 Col_Check(void)
 {
   int cno;
   for(cno=0;cno<4;cno++)
   {
      if(((IOPIN1>>(COL0+cno))&1)==0)
          {
            break;
          }
   }
   return cno;
 }

// Blocking key-scan routine: waits for a key press, determines its
// row/column, looks up the corresponding character in KpmLut, waits
// for key release (with a debounce delay), and returns the key
// character.
u32 Key_Scan(void)
{
   u32 key,cno,rno;
   while(colscan());          // wait until a key is pressed
    rno=Row_Check();
        cno=Col_Check();
        key=KpmLut[rno][cno];
	  while(!colscan());       // wait until key is released
	      delay_ms(100);        // debounce delay
        return key;
}

// Reads a multi-digit numeric entry from the keypad, echoing each
// digit to the LCD. 'c' cancels entry, '+' acts as backspace
// (removes the last entered digit), and '=' confirms/returns the
// entered value.
u32 ReadNum(void)
{
  u8 key;
	s8 count=0;   // number of digits currently entered
  u32 sum=0;    // accumulated numeric value
  while(1)
  {
    
		key=Key_Scan();
		if(key=='c')
		{
			return 'c';   // cancel entry
		}
   if(key>='0' && key<='9' && count<4)   // limit to 4 digits
   {
		 count++;
      sum=(sum*10)+(key-48);
		// cmdLcd(0xc0);
     charLcd(key);
   }
	 if((key=='+') && (count>0))            // backspace last digit
	 {
		 count--;
		 cmdLcd(0x10);      // move cursor left
		 charLcd(' ');      // erase last digit on screen
		 sum=sum/10;
		 cmdLcd(0x10);      // move cursor left again
	 }
   if(key=='=')
    {
          return sum;      // confirm entry
        }
   }
 }
