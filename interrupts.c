/*
 * Filename: interrupts.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: The single interrupt service routine for the whole system.
 *          Three interrupt sources are used:
 *            INT0 (RB0) - pedestrian button pressed  -> remember the request
 *            Timer 2    - 10ms tick                   -> timers.c
 *            ADC        - conversion finished         -> adc.c
 *          The ISR only does short jobs; all the decisions are made in the
 *          main loop.
*/

//Included Files
#include "traffic.h"

/* Connection information
 * RB0 / INT0 - crossing buttons (all four in parallel), active low
*/

//Definitions

//Functions

//Interrupt Functions
void __interrupt() HighInt(void){

    //Pedestrian button - external interrupt 0 (falling edge on RB0)
    if(INTCONbits.INT0IE && INTCONbits.INT0IF){
        INTCONbits.INT0IF = 0;   //Reset the interrupt flag
        ped_button_isr();
    }

    //Timer 2 - 10ms tick
    if(PIE1bits.TMR2IE && PIR1bits.TMR2IF){
        PIR1bits.TMR2IF = 0;     //Reset the interrupt flag
        timer2_isr();
    }

    //ADC - conversion complete
    if(PIE1bits.ADIE && PIR1bits.ADIF){
        PIR1bits.ADIF = 0;       //Reset the ADC interrupt flag
        adc_isr();
    }

    return;
}

//The setup function.
void setup_interrupts(void){
    RCONbits.IPEN = 0;           //No priority levels - everything uses the one ISR above

    //External interrupt 0 - pedestrian button
    INTCON2bits.INTEDG0 = 0;     //Falling edge triggered (button is active low)
    INTCONbits.INT0IF = 0;       //Clear any old flag
    INTCONbits.INT0IE = 1;       //Enable external interrupt 0

    //Timer 2
    PIR1bits.TMR2IF = 0;         //Clear any old flag
    PIE1bits.TMR2IE = 1;         //Timer 2 interrupt enable

    //ADC
    PIR1bits.ADIF = 0;           //Clear any old flag
    PIE1bits.ADIE = 1;           //Enable ADC interrupt

    INTCONbits.PEIE = 1;         //Peripheral interrupts enabled (needed for Timer 2 and ADC)
    INTCONbits.GIE = 1;          //Global interrupts enabled - always do this last
}

// Any other functions used

//Called from the interrupt when a crossing button is pressed
void ped_button_isr(void){
    //Remember the press until the pedestrians get their green (state S7).
    //Presses while the crossing is already green are ignored (cleared in sensors.c).
    //Switch bounce only sets the same flag again, so it does no harm.
    ped_request = 1;
}
