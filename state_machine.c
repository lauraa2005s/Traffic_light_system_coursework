/*
 * Filename: state_machine.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: The finite state machine, written as a switch-case so that every
 *          case matches one state (circle) on the state transition diagram.
 *
 *  Inputs are written in the same order as on the diagram:  M , S , P
 *      M = main road traffic, S = side road request, P = pedestrian request
 *      X = don't care
 *
 *  Each state is held for a set time (T_S1 ... T_S8 in traffic.h).
 *  Only when that time is up are the transition conditions checked.
 *
 *  Transitions (from the state transition diagram):
 *      S1 -> S1   1,X,X  or  0,0,0     (main traffic, or nobody else waiting)
 *      S1 -> S2   0,1,1 / 0,0,1 / 0,1,0 (no main traffic and side or ped waiting)
 *      S2 -> S3   X,X,X
 *      S3 -> S4   0,1,X                 (side road waiting)
 *      S3 -> S7   0,0,1                 (only pedestrians waiting)
 *      S3 -> S8   all other inputs      (back to the main road)
 *      S4 -> S5   X,X,X
 *      S5 -> S6   X,X,X
 *      S6 -> S3   X,X,X
 *      S7 -> S3   X,X,X
 *      S8 -> S1   X,X,X
*/

//Included Files
#include "traffic.h"

/* Connection information
 * None - inputs come from sensors.c, outputs are decoded in outputs.c
*/

//Definitions

//Global Variables
//Time to stay in each state, in ticks. Index = state number (index 0 not used).
const unsigned int state_time[NUM_STATES + 1] = {
    0,      //(not used)
    T_S1,   //S1 Main green (minimum)
    T_S2,   //S2 Main amber
    T_S3,   //S3 All red
    T_S4,   //S4 Side red + amber
    T_S5,   //S5 Side green
    T_S6,   //S6 Side amber
    T_S7,   //S7 Pedestrian green
    T_S8    //S8 Main red + amber
};

//Functions

//The setup function.
void init_state_machine(void){
    //Power up in S3 (everything red) - the safest state. From here, with no
    //inputs, the machine goes S3 -> S8 -> S1 (main road green).
    enter_state(S3);
}

// Any other functions used

//Moves to a new state and loads the time to spend in it
void enter_state(unsigned char new_state){
    current_state = new_state;
    state_timer = state_time[new_state];
}

//Called from the main loop every 10ms
void run_state_machine(void){
    unsigned char M, S, P;

    //Wait until the time for the present state has run out
    if(state_timer > 0){
        state_timer--;
        return;
    }

    //Take a copy of the inputs so they cannot change half way through
    M = main_traffic;
    S = side_request;
    P = ped_request;

    switch(current_state){

        case S1:    //Main GREEN
            if(M == 1){
                //1,X,X - main road traffic present, stay in S1
            }
            else if((S == 1) || (P == 1)){
                enter_state(S2);        //0,1,1 / 0,0,1 / 0,1,0
            }
            else{
                //0,0,0 - nobody waiting anywhere, stay in S1
            }
            break;

        case S2:    //Main AMBER
            enter_state(S3);            //X,X,X
            break;

        case S3:    //ALL RED
            if((M == 0) && (S == 1)){
                enter_state(S4);        //0,1,X - side road goes next
            }
            else if((M == 0) && (S == 0) && (P == 1)){
                enter_state(S7);        //0,0,1 - pedestrians go next
            }
            else{
                enter_state(S8);        //1,X,X or 0,0,0 - back to the main road
            }
            break;

        case S4:    //Side RED + AMBER
            enter_state(S5);            //X,X,X
            break;

        case S5:    //Side GREEN
            enter_state(S6);            //X,X,X
            break;

        case S6:    //Side AMBER
            enter_state(S3);            //X,X,X
            break;

        case S7:    //Pedestrian GREEN
            enter_state(S3);            //X,X,X
            break;

        case S8:    //Main RED + AMBER
            enter_state(S1);            //X,X,X
            break;

        default:    //Should never happen - go to all red to be safe
            enter_state(S3);
            break;
    }
}
