/*
 * Filename: globals.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Defines (creates) every global variable used by the system.
 *          They are declared "extern" in traffic.h so the other files can use them.
 *          Variables changed inside an interrupt are "volatile" so the compiler
 *          always re-reads them instead of using an old copy.
*/

//Included Files
#include "trafficlightsystem.h"

/* Connection information
 * None.
*/

//Global Variables

//---- Written by interrupts (so must be volatile) ----
volatile unsigned char tick_flag = 0;                 //Timer 2 sets this to 1 every 10ms
volatile unsigned char adc_result[NUM_SENSORS] = {0}; //Latest reading (ADRESH) from AN0-AN3
volatile unsigned char adc_channel = 0;               //Which ADC channel is being converted now
volatile unsigned char ped_request = 0;               //1 = a crossing button has been pressed (latched)

//---- Written by the main loop only ----
unsigned char sensor_present[NUM_SENSORS] = {0};      //1 = vehicle detected on that sensor
unsigned char main_traffic = 0;                       //Vehicle on either main road sensor (live)
unsigned char main_input = 0;                         //Input M: main_traffic AND NOT max_green_reached
unsigned char max_green_reached = 0;                  //1 = main road has had its 2 minute maximum green
unsigned int  max_green_timer = 0;                    //Ticks left before the main green maximum is reached
unsigned char side_request = 0;                       //REQUEST flag: vehicle has been seen on the side road (latched)
unsigned char side_served = 0;                        //SERVED flag: side road has had its green this cycle
unsigned char ped_served = 0;                         //SERVED flag: pedestrians have had their green this cycle
unsigned char side_input = 0;                         //Input S: side_request AND NOT side_served
unsigned char ped_input = 0;                          //Input P: ped_request AND NOT ped_served
unsigned char sm_inputs = 0;                          //Inputs packed as binary M S P (e.g. 0b011)
unsigned char current_state = S3;                     //Present state of the state machine
unsigned int  state_timer = 0;                        //Number of ticks left in the present state
