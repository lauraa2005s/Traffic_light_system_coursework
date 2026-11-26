/*
 * Filename: state_machine.c
 * Author: Laura Salomon 
 * Date: 26/11/26
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: The finite state machine, written as a switch-case so that every
 *          case matches one state on the state transition diagram.

//Included Files
#include "trafficlightsystem.h"

/* Connection information
 * None - inputs come from sensors.c, outputs are decoded in outputs.c
*/

//Definitions

//Global Variables
const unsigned int state_time[NUM_STATES + 1] = {
    0,      //(not used)
    T_S1,   //S1 Main green
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
    enter_state(S3);
}

// Any other functions used

//Moves to a new state and loads the time to spend in it
void enter_state(unsigned char new_state){
    current_state = new_state;
    state_timer = state_time[new_state];

    if(new_state == S1){
        max_green_timer = T_S1_MAX;   //Start the 2 minute maximum for this main green
        max_green_reached = 0;
    }
}

//Called from the main loop every 10ms
void run_state_machine(void){
    unsigned char M, S, P;

    //Main green maximum counts the WHOLE time S1 is green
    if((current_state == S1) && (max_green_timer > 0)){
        max_green_timer--;
        if(max_green_timer == 0){
            max_green_reached = 1;    //2 minutes up main traffic no longer holds the green
        }
    }

    //Wait until the minimum time for the present state has run out
    if(state_timer > 0){
        state_timer--;
        return;
    }

    M = main_input;
    S = side_input;
    P = ped_input;

    switch(current_state){

        case S1:    //Main GREEN
            if(M == 1){
                //1,X,X - main road traffic present (and under 2 min), stay in S1
            }
            else if((S == 1) || (P == 1)){
                enter_state(S2);        
            }
            else{
                //0,0,0 nobody waiting anywhere, stay in S1
            }
            break;

        case S2:    //Main AMBER
            enter_state(S3);            
            break;

        case S3:    //ALL RED
            if((M == 0) && (S == 1)){
                enter_state(S4);        
            }
            else if((M == 0) && (S == 0) && (P == 1)){
                enter_state(S7);        
            }
            else{
                enter_state(S8);        //1,X,X, goes back to the main road
            }
            break;

        case S4:    //Side RED + AMBER
            enter_state(S5);           
            break;

        case S5:    //Side GREEN
            enter_state(S6);            
            break;

        case S6:    //Side AMBER
            enter_state(S3);            
            break;

        case S7:    //Pedestrian GREEN
            enter_state(S3);           
            break;

        case S8:    //Main RED + AMBER
            enter_state(S1);            //X,X,X
            break;

        default:    //failure case, it will go to all-red
            enter_state(S3);
            break;
    }
}
