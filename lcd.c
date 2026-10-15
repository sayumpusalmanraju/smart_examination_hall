//lcd.c
//==================================================================
// Section: Character LCD driver (lcd.c)
// Drives a standard HD44780-compatible 16x2 LCD in 8-bit mode using
// separate RS (register select) and EN (enable) control lines.
//==================================================================
#include <LPC21xx.h>
#include "types.h"
#include "defines.h"
#include "lcd_defines.h"
#include "delay.h"
#include "lcd.h"

// Low-level byte write: puts 'byte' on the data bus and pulses the
// Enable (EN) line high->low so the LCD latches the data/command.
void writeLcd(u8 byte)
{
 //write to data pins
 WRITEBYTE(IOPIN0,LCD_DATA,byte);
 //select write operation
// IOCLR0=1<<LCD_RW;
 //provide high to low enable pulse(for latching purpose)
 IOSET0=1<<LCD_EN;
 delay_us(1);
 IOCLR0=1<<LCD_EN;
 delay_ms(2);
}

// Sends a command/instruction byte to the LCD (RS = 0 selects the
// instruction register instead of the data register).
void cmdLcd(u8 opcode)
{
  //clear rs pin for cmd register select
  IOCLR0=1<<LCD_RS;
  //write to cmd register via d0 to d7
  writeLcd(opcode);
}

// One-time LCD initialization sequence: configures the relevant
// GPIO pins as outputs and issues the standard HD44780 power-on
// initialization sequence (function set, display on, clear, entry
// mode) as per the datasheet.
void InitLcd(void)
{
 //cfg data,rs,rw,en as gpio output pins
 IODIR0|=((0xff<<LCD_DATA)|(1<<LCD_RS)|/*(1<<LCD_RW)|*/(1<<LCD_EN));
 delay_ms(15);          // wait for LCD power-on stabilization
 cmdLcd(0x30);           // function set (wake-up #1)
 delay_ms(4);
 delay_us(100);
 cmdLcd(0x30);           // function set (wake-up #2)
 delay_us(100);
 cmdLcd(0x30);           // function set (wake-up #3)
 cmdLcd(0x38);           // 8-bit mode, 2 lines, 5x8 font
 cmdLcd(DSP_ON_CUR_BLK);  // display ON, cursor blinking
 cmdLcd(CLEAR_LCD);       // clear display, reset cursor
 cmdLcd(SHIFT_CUR_RIGHT); // entry mode: auto-increment cursor right
}

// Writes a single printable character to the LCD at the current
// cursor position (RS = 1 selects the data register).
void charLcd(u8 asciival)
{
 //set rs pin for data register select
 IOSET0=1<<LCD_RS;
 //write to in dram via data reg via data pins
 writeLcd(asciival);
}

// Writes a null-terminated string to the LCD, one character at a
// time, starting at the current cursor position.
void strLcd(s8* str)
{
  while(*str)
  charLcd(*str++);
}

// Converts an unsigned 32-bit integer to ASCII and prints it on the
// LCD (no leading zeros, prints "0" for a value of 0).
void u32Lcd(u32 num)
{
  u8 a[10];   // holds decimal digits, least-significant first
  s32 i=0;
	 if(num==0)
  {
   charLcd('0');
  }
  else
  {
    // extract digits in reverse order
    while(num>0)
        {
          a[i++]=(num%10)+48;
          num/=10;
        }
        i--;
        // print digits in correct (most-significant-first) order
        for(;i>-1;i--)
        {
        charLcd(a[i]);
        }
  }
}

// Converts a signed 32-bit integer to ASCII and prints it on the
// LCD, prefixing a '-' sign for negative values.
void s32Lcd(s32 num)
{
 if(num<0)
 {
   charLcd('-');
   num=-num;
 }
 u32Lcd(num);
}

// Prints a floating-point value on the LCD with 'ndp' digits after
// the decimal point.
// NOTE: 'num' is read (in the sign check) before it is assigned a
// value, and it is declared as u32 (unsigned) so the "num<0" check
// can never be true - this looks like a bug in the original code
// and should be reviewed (likely intended to check fnum<0 instead).
void f32Lcd(f32 fnum,u32 ndp)
{
 u32 num;
 s32 i;
 if(num<0.0)
 {
    charLcd('-');
        fnum=-fnum;
 }
 num=fnum;           // truncate to get the integer part
 u32Lcd(num);         // print integer part
 charLcd('.');
 // print 'ndp' fractional digits by repeatedly scaling by 10
 for(i=0;i<ndp;i++)
 {
  fnum=(fnum-num)*10;
  num=fnum;
  charLcd(num+48);
 }
}

// Loads custom character bitmap data ('nbytes' bytes from 'str')
// into the LCD's CGRAM (Character Generator RAM), then returns the
// cursor to the start of line 1 (DDRAM) so normal printing resumes.
void BuildCGram(u8 *str,u32 nbytes)
{
   u32 i;
   cmdLcd(GOTO_CGRAM_START); //go to cgram
   IOSET0=1<<LCD_RS;
   for(i=0;i<nbytes;i++)
   {
      writeLcd(str[i]);
   }
   //goto ddram
   cmdLcd(GOTO_LINE1_POS0);//goto line1 pos0
}

