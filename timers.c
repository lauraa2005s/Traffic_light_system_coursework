/*
 * Filename: timers.c
 * Author: Laura Salomon
 * Date:24/11/26
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: 
 */

//Included Files

#include "trafficlightsystem.h"

//Definitions

//Functions

//The setup function.
void setup_timer2(void){
    T2CONbits.TMR2ON = 0;        //Keep the timer stopped while setting it up
    T2CONbits.T2CKPS = 0b11;     //Set timer 2 prescale to 16
    T2CONbits.TOUTPS = 0b1110;   //Set timer 2 postscale to 15
    TMR2 = 0;                    //Clear the count
    PR2 = 249;                   //Timer 2 resets to 0 when it matches PR2 (250 counts)
    //(Timer 2 interrupt is enabled in setup_interrupts(), timer is started in main())
}

// Any other functions used

//called from the interrupt every 10ms
void timer2_isr(void){
    
    tick_flag = 1;               //Tell the main loop that 10ms has passed
}
