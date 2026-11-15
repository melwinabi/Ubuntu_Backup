#include<lpc214x.h>

#define PLOCK 0x00000400

int main(void)

{

/* PPLE = 1 and PPLC = 0 so PLL0 will be enabled but not connected.*/

PLL0CON = 0x01;

/* PLL0CFG is set as per the table mentioned above. */

PLL0CFG = 0x24;

/* Feed Sequence for locking PLL0 to desired frequency */

PLL0FEED = 0xAA;

PLL0FEED = 0x55;

/* Check whether the PLL0 has locked on to the desired frequency by reading the lock bit

in the PPL0STAT register */

while( !( PLL0STAT & PLOCK ));

/* Now enable(again) and connect the PLL0 */

PLL0CON = 0x03;

/* Feed Sequence for connecting the PLL0 as system clock */

PLL0FEED = 0xAA;

PLL0FEED = 0x55;

/* Now the System Clock is running @ 60 MHz! Next step is to set the Peripheral Clock at same frequency as System Clock */

VPBDIV = 0x01;

while(1);

}








//https://www.electronicshub.org/arm-pll-tutorial/
