// this function used to check the password is correct or not
// if same return 1 if not then return 0.
#include "types.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "delay.h"

extern u32 save_pass;   // globally stored password to compare against

u8 password(void)
{
    u32 temp = 0;     // holds the digits entered by the user (built up digit by digit)
    u32 count = 4;     // number of digits still needed (password is 4 digits long)
    u32 temp1 = 0;     // holds the key that was just pressed

    cmdLcd(CLEAR_LCD);          // clear the LCD screen
    strLcd("enter the pin:");   // prompt user to enter pin

    // ---- Loop to collect up to 4 digits of the PIN ----
    while(count)
    {
        temp1 = Key_Scan();   // read a key press from the keypad

        // only process the key if it's a digit, cancel, or backspace
        if((temp1 >= '0' && temp1 <= '9') || temp1 == 'c' || temp1 == '+')
        {
            if(temp1 == 'c')
            {
                return 'c';   // 'c' pressed -> cancel, exit function immediately
            }

            // '+' is used as backspace, only allowed if at least one digit
            // has already been entered (count < 4) and not all digits used up (count > 0)
            if(temp1 == '+' && (count < 4 && count > 0))
            {
                again:                     // label used by goto below (from confirm loop)
                cmdLcd(0x10);              // move LCD cursor one position back
                charLcd(' ');              // overwrite the last '*' with a blank space
                cmdLcd(0x10);              // move cursor back again to sit on the blank
                temp /= 10;                // remove the last entered digit from temp
                count++;                   // one less digit entered, so increment count back
            }
            else if(temp1 >= '0' && temp1 <= '9')
            {
                count--;                   // one more digit entered, decrement remaining count
                charLcd('*');              // show '*' on LCD instead of the actual digit (mask input)
                temp = temp * 10 + temp1 - '0';   // shift existing digits left and add new digit
            }
        }
    }

    // ---- After 4 digits entered, wait for user to press '=' to confirm ----
    while((temp1 = Key_Scan()) != '=')
    {
        if(temp1 == '+')
        {
            goto again;   // jump back up to backspace handling code
                       
        }
        if(temp1 == 'c')
        {
            return 'c';   // 'c' pressed while waiting for '=' -> cancel/go back
        }
    }

    // ---- Compare entered PIN with the saved password ----
    if(temp == save_pass)
    {
        cmdLcd(GOTO_LINE2_POS0);        // move cursor to line 2
        strLcd("valid password");       // show success message
        delay_ms(1000);                 // pause so user can read the message
        return 1;                       // password correct
    }

    return 0;   // password incorrect
}

// It provides to reset the password
void edit_password(void)
{
    s32 key;

    cmdLcd(CLEAR_LCD);
    strLcd("Enter New Password");
    cmdLcd(GOTO_LINE2_POS0);
    strLcd("4 digit num: ");

    key = ReadNum();   // read new password entered by user as a number

    cmdLcd(GOTO_LINE3_POS0);

    // check that entered value is a valid 4-digit number (1000-9999)
    if((key >= 1000) && (key <= 9999))
    {
        save_pass = key;                     // store new password in global variable
        strLcd("updated sucessfully");       // confirm update on LCD
       
    }

    else if(key == 'c')
    {
        return;   // user cancelled, exit without changes
    }
    else
    {
       
        strLcd("invalid new pass");
    }

    delay_ms(1000);
    cmdLcd(CLEAR_LCD);
}
