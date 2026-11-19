/*
 * Filename: sensors.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Signal conditioning. Turns the four analogue sensor readings into
 *          the three digital inputs used by the state machine:
 *            M = main_traffic  - a vehicle is on either main road sensor (live)
 *            S = side_request  - a vehicle has been seen on either side road
 *                                sensor (latched until the side road gets green)
 *            P = ped_request   - a crossing button has been pressed
 *                                (latched until the pedestrians get green)
 *          Hysteresis is used on each sensor:
 *            above 1.4V -> vehicle present
 *            below 0.5V -> no vehicle
 *            in between -> keep the previous decision (stops flickering)
*/

//Included Files
#include "traffic.h"

/* Connection information
 * Uses adc_result[] filled in by adc.c (AN0-AN3 = RA0-RA3)
*/

//Definitions

//Functions

// Any other functions used

//Called from the main loop every 10ms
void process_sensors(void){
    unsigned char i;
    unsigned char reading;
    unsigned char side_now;

    //1. Convert each analogue reading into vehicle / no vehicle (with hysteresis)
    for(i = 0; i < NUM_SENSORS; i++){
        reading = adc_result[i];                 //Take a copy (it is changed by the ADC interrupt)
        if(reading >= VEHICLE_ON_LEVEL){
            sensor_present[i] = 1;               //More than 1.4V - vehicle present
        }
        else if(reading <= VEHICLE_OFF_LEVEL){
            sensor_present[i] = 0;               //Less than 0.5V - no vehicle
        }
        //Otherwise leave sensor_present[i] as it was
    }

    //2. Main road input (M) - both directions are treated as one road
    main_traffic = sensor_present[MAIN_SENSOR_1] | sensor_present[MAIN_SENSOR_2];

    //3. Side road input (S) - remembered until it is answered
    side_now = sensor_present[SIDE_SENSOR_1] | sensor_present[SIDE_SENSOR_2];
    if((current_state == S4) || (current_state == S5)){
        side_request = 0;                        //Side road is being served - clear the request
    }
    else if(side_now){
        side_request = 1;                        //Remember that a side road vehicle is waiting
    }

    //4. Pedestrian input (P) - set by the INT0 interrupt, cleared once served
    if(current_state == S7){
        ped_request = 0;                         //Pedestrians have green - clear the request
    }

    //5. Pack the inputs as M,S,P (same order as the state transition diagram)
    //   so they can be watched in the MPLAB X watch window while testing
    sm_inputs = (unsigned char)((main_traffic << 2) | (side_request << 1) | ped_request);
}
