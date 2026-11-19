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
 *  Timer 2 calculation (4MHz crystal):
 *      Instruction clock = Fosc/4         = 1,000,000 Hz
 *      Prescale 1:4                       =   250,000 Hz
 *      PR2 = 249 (counts 0-249 = 250)     =     1,000 Hz
 *      Postscale 1:10                     =       100 Hz  -> 10ms per interrupt
 *
 *  If the board uses a 20MHz crystal instead, use prescale 1:16, PR2 = 249,
 *  postscale 1:10 (gives 125Hz = 8ms) and change TICKS_PER_SECOND to 125.
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
    T2CONbits.T2CKPS = 0b01;     //Set timer 2 prescale to /4
    T2CONbits.TOUTPS = 0b1001;   //Set timer 2 postscale to /10
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
