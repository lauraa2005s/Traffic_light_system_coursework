/*
 * Filename: hardware.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Sets the direction (input/output) of the Port B, D and E pins and
 *          turns on the internal pull-up needed by the pedestrian button on RB0.
 *          (Port A and the analogue/digital selection are done in adc.c.)
*/

//Included Files
#include "traffic.h"

/* Connection information
 * Port A: RA0-RA3 analogue inputs (traffic sensors), RA4-RA5 unused (inputs)
 * Port B: RB0 input (crossing buttons), RB1 unused, RB2-RB7 outputs
 * Port D: RD0-RD7 all outputs (lights)
 * Port E: RE0-RE1 outputs (pedestrian green/red), RE2 unused
*/

//Definitions
#define PORTB_CFG  TRISB
#define PORTD_CFG  TRISD
#define PORTE_CFG  TRISE

//Functions

//The setup function.
void setup_ports(void){
    //Port A - traffic sensors
    //(Port A direction and the analogue/digital pin selection (PCFG) are set
    // in setup_adc() in adc.c, following the ADC lab code)

    //Port B - crossing button and wait / crossing lights
    PORTB_CFG = 0b00000011;      //RB0 input (button), RB1 unused input, RB2-RB7 outputs
    INTCON2bits.RBPU = 0;        //Turn ON Port B weak pull-ups - the button has no external pull-up,
                                 //it just connects RB0 to ground when pressed (pull-ups only act on inputs)

    //Port D - main and side road lights + pedestrian lights
    PORTD_CFG = 0b00000000;      //All outputs

    //Port E - pedestrian lights
    PORTE_CFG = 0b00000000;      //RE0-RE2 outputs, PSPMODE = 0 (normal I/O, not parallel slave port)

    //Start with every output off
    LATB = 0x00;
    LATD = 0x00;
    LATE = 0x00;
}
