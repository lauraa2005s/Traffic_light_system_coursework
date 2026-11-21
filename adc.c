/*
 * Filename: adc.c
 * Author:
 * Date:
 * Revision Number: 2
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Reads the four analogue traffic sensors on Port A bits 0-3.
 *          Developed from the ADC lab code (ADC_main.c, Dr R Walters), which
 *          read one analogue value from Port A bit 0 using the ADC interrupt
 *          and restarted the conversion each time it finished.
 *
 *          Same as the lab:
 *            - Port A set to inputs, PCFG = 0b0010 (five analogue inputs)
 *            - ADC clock = fosc/64 (ADCS2:ADCS0 = 1,1,0)
 *              At fosc = 24MHz: Tad = 64 / 24MHz = 2.67us. The datasheet
 *              needs Tad of at least 1.6us, so fosc/64 is correct
 *              (fosc/32 would give 1.33us - too fast)
 *            - Left justified, so the top 8 bits of the result are in ADRESH
 *            - ADC interrupt enabled, ISR clears ADIF and restarts the
 *              conversion (ADCON0bits.GO = 1)
 *            - First conversion started from main()
 *
 *          Added for the traffic lights:
 *            - The lab only read AN0. We need four sensors, so each time a
 *              conversion finishes the result is saved in adc_result[] and
 *              the next channel is selected (AN0 -> AN1 -> AN2 -> AN3 -> AN0)
 *            - After changing channel the ADC needs time for its holding
 *              capacitor to charge to the new input (acquisition time,
 *              about 12.86us from the datasheet), so we wait 20us before
 *              the conversion is restarted
 *            - The lab didn't do anything with the value; here sensors.c
 *              compares it with the 1.4V / 0.5V thresholds
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
#define ACQUISITION_TIME_US 20   //Wait after changing channel (datasheet minimum about 12.86us)

//Functions

//The setup function.
void setup_adc(void){
    //ADC Set Up
    ADCPORT_CFG = 0b0111111;     //set the port to all inputs
    ADCON1bits.PCFG = 0b0010;    //Configures five analogue inputs (AN0-AN4), RE0-RE2 stay digital
    adc_channel = MAIN_SENSOR_1;
    ADCON0bits.CHS = adc_channel;//Select Analogue input zero
    ADCON0bits.ADCS0 = 0;        //Clock select lsb = 0
    ADCON0bits.ADCS1 = 1;        //Clock select mid bit = 1
    ADCON1bits.ADCS2 = 1;        //Clock select msb = 1  fadc = fosc/64
    ADCON1bits.ADFM = 0;         //Left justify output
    ADCON0bits.ADON = 1;         //Turn the ADC on

    //Interrupt Set-up
    PIR1bits.ADIF = 0;           //Clear any old ADC flag
    PIE1bits.ADIE = 1;           //Enable ADC interrupt
    //(Global and peripheral enables are set in setup_interrupts() in interrupts.c,
    // because INT0 and Timer 2 share them - see the note there)
}

// Any other functions used

//Called from the ADC interrupt (interrupts.c) when a conversion has finished.
//The ISR then restarts the conversion, exactly as in the lab code.
void adc_isr(void){
    adc_result[adc_channel] = ADRESH;   //Save the 8 bit result for this sensor

    adc_channel++;                      //Move on to the next sensor
    if(adc_channel >= NUM_SENSORS){
        adc_channel = 0;                //Back to AN0 after AN3
    }
    ADCON0bits.CHS = adc_channel;       //Select the next analogue input

    __delay_us(ACQUISITION_TIME_US);    //Let the input settle before the next conversion
}
