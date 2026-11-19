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
#include "traffic.h"

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
unsigned char main_traffic = 0;                       //Input M: vehicle on either main road sensor
unsigned char side_request = 0;                       //Input S: vehicle has been seen on the side road (latched)
unsigned char sm_inputs = 0;                          //Inputs packed as binary M S P (e.g. 0b011)
unsigned char current_state = S3;                     //Present state of the state machine
unsigned int  state_timer = 0;                        //Number of ticks left in the present state
