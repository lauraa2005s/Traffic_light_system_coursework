/*
 * Filename: sensors.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Signal conditioning. Turns the four analogue sensor readings and
 *          the crossing button into the three digital inputs used by the
 *          state machine:
 *            M = main_input   - a vehicle is on either main road sensor (live)
 *                               AND the 2 minute main green maximum has not
 *                               been reached
 *            S = side_input   - side road is waiting AND has not been served yet
 *            P = ped_input    - pedestrians are waiting AND have not been served yet
 *
 *          Flags used:
 *            side_request - REQUEST flag: a side road vehicle has been seen
 *                           (set by the sensor, cleared while side is served S4-S6)
 *            ped_request  - REQUEST flag: a crossing button has been pressed
 *                           (set by INT0, cleared while pedestrians are served S7)
 *            side_served  - SERVED flag: side road has had its green this cycle
 *                           (set in S4-S6, cleared when main road goes green S1)
 *            ped_served   - SERVED flag: pedestrians have had their green this cycle
 *                           (set in S7, cleared when main road goes green S1)
 *
 *          The served flags stop a restricted loop around S3. Without them a
 *          car waiting at the side road after S6, or a button press during
 *          the all red after S7, would send S3 straight back into the same
 *          phase (S3->S4->S5->S6->S3... or S3->S7->S3->S7...) and the main
 *          road would never get its turn. With them, each of the side road and
 *          the pedestrians gets at most ONE turn between main road greens. A
 *          request made while locked out is still remembered (and the wait
 *          lights stay on) - it is just served on the next cycle.
 *
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

    //3. Side road REQUEST flag - remembered until it is answered
    side_now = sensor_present[SIDE_SENSOR_1] | sensor_present[SIDE_SENSOR_2];
    if((current_state == S4) || (current_state == S5) || (current_state == S6)){
        side_request = 0;                        //Side road is being served - clear the request
        side_served  = 1;                        //...and mark it as served this cycle
    }
    else if(side_now){
        side_request = 1;                        //Remember that a side road vehicle is waiting
    }

    //4. Pedestrian REQUEST flag - set by the INT0 interrupt, cleared once served
    if(current_state == S7){
        ped_request = 0;                         //Pedestrians have green - clear the request
        ped_served  = 1;                         //...and mark them as served this cycle
    }

    //5. SERVED flags are cleared when the main road gets its green,
    //   so the side road and pedestrians can be served again next cycle
    if(current_state == S1){
        side_served = 0;
        ped_served  = 0;
    }

    //6. Inputs to the state machine
    //   M: main traffic only holds the green until the 2 minute maximum is reached
    //      (max_green_reached is set by the state machine, reset each time S1 starts)
    //   S, P: waiting AND not already served this cycle
    main_input = (main_traffic && !max_green_reached) ? 1 : 0;   //M
    side_input = (side_request && !side_served) ? 1 : 0;         //S
    ped_input  = (ped_request  && !ped_served)  ? 1 : 0;         //P

    //7. Pack the inputs as M,S,P (same order as the state transition diagram)
    //   so they can be watched in the MPLAB X watch window while testing
    sm_inputs = (unsigned char)((main_input << 2) | (side_input << 1) | ped_input);
}
