/*
 * Filename: adc.c
 * Author:
 * Date:
 * Revision Number: 3
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Set-up for reading the four analogue traffic sensors (RA0-RA3).
 *          All of the ADC and Timer 1 set-up is in setup_adc(), so it is
 *          clear what is set-up and what is operational.
 *          Operation (in the interrupt in interrupts.c):
 *            - Timer 1 overflows every 10.92ms and starts a conversion
 *            - When the conversion finishes, the ADC interrupt saves ADRESH
 *              in road_sensors[] and switches to the next channel
 *          Each sensor is therefore read every 4 x 10.92ms = 43.7ms.
 *
 *          Timer 1 (16 bit), fosc = 24MHz:
 *            fosc/4 = 6MHz, prescale 1:1, counts 0-65535 then overflows
 *            65536 / 6,000,000 = 10.92ms per interrupt (no reload needed)
 *          The channel is switched ~10.9ms before the next conversion starts,
 *          far longer than the ~12.86us acquisition time the datasheet needs,
 *          so no acquisition delay is required.
 *
 *          ADC clock = fosc/64: Tad = 64 / 24MHz = 2.67us. The datasheet
 *          needs Tad of at least 1.6us (fosc/32 would give 1.33us - too fast).
*/

//Included Files
#include "trafficlightsystem.h"

/* Connection information
 * ADC Inputs are from Port A bits 0-3
 * AN0 = RA0 main road sensor 1
 * AN1 = RA1 side road sensor 1
 * AN2 = RA2 main road sensor 2
 * AN3 = RA3 side road sensor 2
*/

//Definitions
#define ADCPORT_CFG TRISA        //Direction setting

//Functions

//The setup function.
void setup_adc(void){
    //Port set-up
    ADCPORT_CFG = 0b00001111;    //Set RA0-RA3 as inputs

    //ADC set-up
    ADCON1bits.PCFG = 0b0010;    //Configure AN0-AN4 pins for analogue operation (RE0-RE2 stay digital)
    ADCON1bits.ADFM = 0;         //Left justified formatting
    ADCON0bits.ADCS0 = 0;        //Clock select lsb = 0
    ADCON0bits.ADCS1 = 1;        //Clock select mid bit = 1
    ADCON1bits.ADCS2 = 1;        //Clock select msb = 1  fadc = fosc/64
    current_channel = 0;
    ADCON0bits.CHS = current_channel; //Start at channel 0 (AN0)
    ADCON0bits.ADON = 1;         //Turn on the ADC module

    //Timer 1 set-up (paces the ADC readings)
    T1CONbits.TMR1ON = 0;        //Keep Timer 1 stopped while setting it up
    T1CONbits.TMR1CS = 0;        //Clock source = internal instruction clock (fosc/4 = 6MHz)
    T1CONbits.T1CKPS = 0b00;     //Prescale 1:1
    T1CONbits.T1OSCEN = 0;       //Timer 1 oscillator off (not used)
    TMR1H = 0;                   //Clear the count (high byte)
    TMR1L = 0;                   //Clear the count (low byte)
    //(Timer 1 is started in main() with T1CONbits.TMR1ON = 1)

    //Interrupt set-up
    PIR1bits.ADIF = 0;           //Start with ADC flag clear
    PIE1bits.ADIE = 1;           //Enable ADC interrupt
    PIR1bits.TMR1IF = 0;         //Start with Timer 1 flag clear
    PIE1bits.TMR1IE = 1;         //Enable Timer 1 interrupt
    //(PEIE and GIE are set in setup_interrupts() in interrupts.c)
}
