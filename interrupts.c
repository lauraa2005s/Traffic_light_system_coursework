/*
 * Filename: interrupts.c
 * Author: Laura Salomon 
 * Date: 23/11/26
 * Revision Number: 0.2
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary:
*/

//Included Files
#include "trafficlightsystem.h"

/* Connection information
 * RB0 / INT0 - crossing buttons (all four in parallel), active low
*/

//Definitions

//Functions

//Interrupt Functions
void __interrupt() HighInt(void){

    //ped button - external interrupt 0 (falling edge on RB0)
    if(INTCONbits.INT0IE && INTCONbits.INT0IF){
        INTCONbits.INT0IF = 0;   //Reset the interrupt flag
        ped_button_isr();
    }

    //timer 2 - 10ms tick
    if(PIE1bits.TMR2IE && PIR1bits.TMR2IF){
        PIR1bits.TMR2IF = 0;     //Reset the interrupt flag
        timer2_isr();
    }

    //Tmr1 triggers the next ADC conversion every 10.92ms
    if(PIE1bits.TMR1IE && PIR1bits.TMR1IF){
        PIR1bits.TMR1IF = 0;     //Manually clear the Timer 1 flag bit
        ADCON0bits.GO_DONE = 1;  //Start the ADC
    }

    //ADC conversion complete
    if(PIE1bits.ADIE && PIR1bits.ADIF){
        PIR1bits.ADIF = 0;                       //Manually clear the ADC flag bit
        road_sensors[current_channel] = ADRESH;  //Read the result

        //Move to the next road sensor
        current_channel++;
        if(current_channel > 3){current_channel = 0;}

        //Switch the ADC hardware to the next channel
        ADCON0bits.CHS = current_channel;
    }

    return;
}

//The setup function
void setup_interrupts(void){
    RCONbits.IPEN = 0;           //No priority levels as everything uses the one ISR above

    //external interrupt 0, pedestrian button
    INTCON2bits.INTEDG0 = 0;     //Falling edge triggered (button is active low)
    INTCONbits.INT0IF = 0;       //Clear any old flag
    INTCONbits.INT0IE = 1;       //Enable external interrupt 0

    //TMR2 2
    PIR1bits.TMR2IF = 0;         //Clear any old flag
    PIE1bits.TMR2IE = 1;         //Timer 2 interrupt enable

    //Timer 1 and ADC interrupt enables (TMR1IE, ADIE) are set in setup_adc() in adc.c
    INTCONbits.PEIE = 1;         //Peripheral interrupts enabled (needed for Timer 1, Timer 2 and ADC)
    INTCONbits.GIE = 1;          //Global interrupts enable
}

// Any other functions used

//Called from the interrupt when a crossing button is pressed
void ped_button_isr(void){
    
    ped_request = 1;
}
