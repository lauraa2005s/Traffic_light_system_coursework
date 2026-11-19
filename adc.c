/*
 * Filename: adc.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Analogue to digital conversion of the four traffic sensors.
 *          The ADC works its way round AN0 -> AN1 -> AN2 -> AN3 -> AN0 ...
 *          Timer 2 starts a conversion every 10ms. When it finishes, the ADC
 *          interrupt stores the result and switches to the next channel, so
 *          the input has ~10ms to settle (acquisition time) before the next
 *          conversion. Each sensor is therefore read every 40ms.
*/

//Included Files
#include "traffic.h"

/* Connection information
 * AN0 = RA0 main road sensor 1
 * AN1 = RA1 side road sensor 1
 * AN2 = RA2 main road sensor 2
 * AN3 = RA3 side road sensor 2
*/

//Definitions

//Functions

//The setup function.
void setup_adc(void){
    //(Pin directions and ADCON1 PCFG are set in setup_ports() in hardware.c)
    adc_channel = MAIN_SENSOR_1;
    ADCON0bits.CHS = adc_channel; //Start on analogue input zero
    ADCON0bits.ADCS0 = 0;         //Clock select lsb = 0
    ADCON0bits.ADCS1 = 1;         //Clock select mid bit = 1
    ADCON1bits.ADCS2 = 1;         //Clock select msb = 1  -> fadc = fosc/64
    ADCON1bits.ADFM = 0;          //Left justify - top 8 bits of the result are in ADRESH
    ADCON0bits.ADON = 1;          //Turn the ADC on
    //(The ADC interrupt is enabled in setup_interrupts() in interrupts.c)
}

// Any other functions used

//Starts a conversion on the currently selected channel (called by Timer 2 every 10ms)
void adc_start_conversion(void){
    if(ADCON0bits.GO == 0){       //Only start if the last conversion has finished
        ADCON0bits.GO = 1;
    }
}

//Called from the interrupt when a conversion has finished
void adc_isr(void){
    adc_result[adc_channel] = ADRESH;   //Save the 8 bit result for this sensor

    adc_channel++;                      //Move on to the next sensor
    if(adc_channel >= NUM_SENSORS){
        adc_channel = 0;                //Back to AN0 after AN3
    }
    ADCON0bits.CHS = adc_channel;       //Select it now so it settles before the next GO
}
