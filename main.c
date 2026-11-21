/*
 * Filename: main.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Traffic light controller for a crossroads (one main road, one side
 *          road and a pedestrian crossing on all four roads) following the UK
 *          sequence Red, Red-Amber, Green, Amber, Red.
 *          main() sets everything up and then, every 10ms tick, runs:
 *            Signal conditioning -> State machine -> Output decoding
 *          which follows the block diagram in the coursework brief.
 *
 *  Project files:
 *      config.c        configuration bits
 *      globals.c       global variables
 *      hardware.c      port directions, analogue/digital pins, pull-ups
 *      adc.c           ADC + Timer 1 set-up for the four traffic sensors
 *      timers.c        Timer 2 10ms tick
 *      interrupts.c    interrupt service routine + interrupt set-up
 *      sensors.c       signal conditioning (thresholds, hysteresis, latching)
 *      state_machine.c the finite state machine (switch-case)
 *      outputs.c       output decoding (state table) + safety check
 *      main.c          this file
 *      trafficlightsystem.h  shared definitions and prototypes
*/

//Configuration statements
//(These are all in config.c)

//Included Files
#include "trafficlightsystem.h"

/* Connection information
 * See trafficlightsystem.h for the full pin list.
*/

//Definitions

//Global Variables
//(These are all in globals.c)

//Functions
void setup(void);

//Interrupt Functions (if used)
//(The interrupt function is in interrupts.c)

//Main Function
void main(void){
//Local Variables

//Call initialisation function if used
    setup();

//Local default set-up parameters
    T2CONbits.TMR2ON = 1;        //Start Timer 2 - the 10ms ticks begin here
    T1CONbits.TMR1ON = 1;        //Start Timer 1 - the ADC readings begin here

//The actual code
/* Every time Timer 2 sets tick_flag (every 10ms) the main loop:
 *   1. process_sensors()   - turns the ADC readings and button press into M,S,P
 *   2. run_state_machine() - counts down the state time, then decides the next state
 *   3. update_outputs()    - decodes the state number into the lights
 * Between ticks the processor just waits; the interrupts keep the ADC and
 * pedestrian button up to date in the background (Timer 1 starts a new ADC
 * conversion every 10.92ms).
*/
    while(1){
        if(tick_flag){
            tick_flag = 0;           //Clear the flag ready for the next tick
            process_sensors();       //Signal conditioning
            run_state_machine();     //State machine
            update_outputs();        //Output decoding
        }
    } //end of while loop
} //End of the main function

//The setup function.
void setup(void){
    setup_ports();               //Pin directions, analogue pins, pull-ups (hardware.c)
    all_red();                   //Make sure every light is red straight away (outputs.c)
    setup_adc();                 //Port A inputs, ADC on AN0, Timer 1, ADC + Timer 1 interrupts enabled (adc.c)
    setup_timer2();              //10ms tick (timers.c)
    init_state_machine();        //Start in S3 - all red (state_machine.c)
    setup_interrupts();          //Enable INT0 and Timer 2 interrupts, then PEIE and GIE - last (interrupts.c)
}

// Any other functions used
