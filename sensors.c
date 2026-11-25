/*
 * Filename: sensors.c
 * Author:Laura Salomon
 * Date:25/11/26
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary:
 */

//Included Files
#include "trafficlightsystem.h"

/* Connection information
 * Uses road_sensors[] filled in by the ADC interrupt (AN0-AN3 = RA0-RA3)
*/

//Definitions

//Functions

//this is called from the main loop every 10ms
void process_sensors(void){
    unsigned char i;
    unsigned char reading;
    unsigned char side_now;

    //this converts each analogue reading into vehicle or no vehicle 
    for(i = 0; i < NUM_SENSORS; i++){
        reading = road_sensors[i];                 
        if(reading >= VEHICLE_ON_LEVEL){
            sensor_present[i] = 1;               //More than 1.4V  means vehicle present
        }
        else if(reading <= VEHICLE_OFF_LEVEL){
            sensor_present[i] = 0;               //Less than 0.5V means no vehicle
        }
  
    }

    //Main road input M - both directions are treated as in-sync
    main_traffic = sensor_present[MAIN_SENSOR_1] | sensor_present[MAIN_SENSOR_2];

    //side road request flags remembered until answered
    side_now = sensor_present[SIDE_SENSOR_1] | sensor_present[SIDE_SENSOR_2];
    if((current_state == S4) || (current_state == S5) || (current_state == S6)){
        side_request = 0;                        //Side road is being served - clear the request
        side_served  = 1;                        //side road served
    }
    else if(side_now){
        side_request = 1;                        //remembers that a side road vehicle is waiting
    }

    //4. Pedestrian REQUEST flag - set by the INT0 interrupt, cleared once served
    if(current_state == S7){
        ped_request = 0;                         //Pedestrians have green - clear the request
        ped_served  = 1;                         //pedestrian served in this cycle
    }

    //served flags are cleared when the main road gets its green
    if(current_state == S1){
        side_served = 0;
        ped_served  = 0;
    }

    //state machine inputs
    main_input = (main_traffic && !max_green_reached) ? 1 : 0;   //M
    side_input = (side_request && !side_served) ? 1 : 0;         //S
    ped_input  = (ped_request  && !ped_served)  ? 1 : 0;         //P

    sm_inputs = (unsigned char)((main_input << 2) | (side_input << 1) | ped_input);
}
