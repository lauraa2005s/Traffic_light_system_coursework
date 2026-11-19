/*
 * Filename: timers.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Timer 2 produces a regular 10ms "tick". Every tick it sets tick_flag
 *          so the main loop knows to run. All of the state timings are
 *          counted in these 10ms ticks.
 *
 *  Timer 2 calculation (fosc = 24MHz crystal):
 *      Instruction clock = fosc/4         = 6,000,000 Hz
 *      Prescale 1:16                      =   375,000 Hz
 *      PR2 = 249 (counts 0-249 = 250)     =     1,500 Hz
 *      Postscale 1:15                     =       100 Hz  -> 10ms per interrupt
 *
 *  The prescale (/16) and postscale (/15) are the same as the Timer 2 lab code.
 *  The lab used PR2 = 255 and reloaded TMR2 = 6 in the interrupt to get 250
 *  counts. Here PR2 = 249 is used instead: Timer 2 resets itself to 0 when it
 *  matches PR2, so EVERY period is exactly 250 counts and no reload is needed.
 *  (With the lab method only the first of the 15 periods is 250 counts - the
 *  other 14 count 0-255 = 256, so the tick would be about 10.2ms, not 10ms.)
*/

//Included Files
#include "traffic.h"

/* Connection information
 * None - Timer 2 is internal.
*/

//Definitions

//Functions

//The setup function.
void setup_timer2(void){
    T2CONbits.TMR2ON = 0;        //Keep the timer stopped while setting it up
    T2CONbits.T2CKPS = 0b11;     //Set timer 2 prescale to /16
    T2CONbits.TOUTPS = 0b1110;   //Set timer 2 postscale to /15
    TMR2 = 0;                    //Clear the count
    PR2 = 249;                   //Timer 2 resets to 0 when it matches PR2 (250 counts)
    //(Timer 2 interrupt is enabled in setup_interrupts(), timer is started in main())
}

// Any other functions used

//Called from the interrupt every 10ms
void timer2_isr(void){
    //No need to reload TMR2 - it automatically goes back to 0 when it matches PR2
    tick_flag = 1;               //Tell the main loop that 10ms has passed
}
