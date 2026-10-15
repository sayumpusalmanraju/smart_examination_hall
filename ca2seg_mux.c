//ca2seg_mux.c
//==================================================================
// Section: 2-digit multiplexed 7-segment display driver (seg.c)
// Drives two common-anode 7-segment displays that share a single
// segment data bus, using digit-select (anode) lines DSEL1/DSEL2
// and time-division multiplexing (persistence of vision).
//==================================================================

#include <LPC21xx.h>
#include "types.h"
#include "seg_defines.h"
#include "defines.h"
// Lookup table: index = digit value (0-9), value = common-anode
// segment pattern (active-low) for that digit.
u8 segLUT[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x98};

// Configures the segment data port and digit-select pins as GPIO
// outputs.
void init_7segs(void)
{
  WRITEBYTE(IODIR1,ca7seg_2_mux,0xFF);
  //WRITENBITS(IODIR0,DSEL1,16,17);
  IODIR0|=1<<DSEL1 | 1<<DSEL2;
}

u8 scount;  // free-running counter used to alternate/refresh digits

// Must be called periodically (e.g. from a timer ISR or main loop)
// to refresh the 2-digit display. Alternates between driving the
// tens digit and the units digit of 'num' so both appear lit due
// to persistence of vision.
void disp_2_mux_segs(u8 num)
{
   
        scount++; 
			 if(scount==200)
			 {
        scount=0;
			 }				

        if (scount % 2 == 0)
        {
            /* TENS digit */
            IOCLR0 = (1 << DSEL1);               /* units anode OFF         */
   
					 WRITEBYTE(IOPIN1,ca7seg_2_mux,segLUT[num%10]);
     
            IOSET0 = (1 << DSEL2);               /* tens anode ON  */
        }
        else
        {
            /* UNITS digit */
            IOCLR0 = (1 <<DSEL2);                           /* tens anode OFF */
            WRITEBYTE(IOPIN1,ca7seg_2_mux,segLUT[num/10]);  /* units pattern */
            IOSET0 = (1 << DSEL1);                          /* units anode ON */
        }

      

}

