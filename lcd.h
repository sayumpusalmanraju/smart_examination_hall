#ifndef LCD_H
#define LCD_H

#include "types.h"

void writeLcd(u8);
void cmdLcd(u8);
void InitLcd(void);
void charLcd(u8);
void strLcd(s8*);
void u32Lcd(u32);
void s32Lcd(s32);
void f32Lcd(f32, u32);
void BuildCGram(u8*, u32);

#endif // LCD_H

