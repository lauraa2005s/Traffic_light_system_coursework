/*
 * Filename: outputs.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Output decoding. Turns the state number into the light outputs
 *          using look-up tables that are a direct copy of the state table:
 *
 *          State | Main RAG | Side RAG | Ped RG
 *            1   |   001    |   100    |   10
 *            2   |   010    |   100    |   10
 *            3   |   100    |   100    |   10
 *            4   |   100    |   110    |   10
 *            5   |   100    |   001    |   10
 *            6   |   100    |   010    |   10
 *            7   |   100    |   100    |   01
 *            8   |   110    |   100    |   10
 *
 *          A safety check stops two conflicting greens ever being output.
 *          The four wait lights are on while a pedestrian request is waiting.
*/

//Included Files
#include "trafficlightsystem.h"

/* Connection information
 * Main road  R/A/G = RD7/RD6/RD5
 * Side road  R/A/G = RD4/RD3/RD2
 * Ped red    = RD1, RE1, RB3
 * Ped green  = RD0, RE0, RB2
 * Wait lights = RB4, RB5, RB6, RB7
*/

//Definitions
//Bit positions inside the table entries
#define RED_BIT_3    0b100   //R of RAG
#define AMBER_BIT_3  0b010   //A of RAG
#define GREEN_BIT_3  0b001   //G of RAG
#define RED_BIT_2    0b10    //R of RG
#define GREEN_BIT_2  0b01    //G of RG

//Global Variables
//Index = state number (index 0 not used)            S1     S2     S3     S4     S5     S6     S7     S8
const unsigned char main_rag[NUM_STATES + 1] = {0, 0b001, 0b010, 0b100, 0b100, 0b100, 0b100, 0b100, 0b110};
const unsigned char side_rag[NUM_STATES + 1] = {0, 0b100, 0b100, 0b100, 0b110, 0b001, 0b010, 0b100, 0b100};
const unsigned char ped_rg[NUM_STATES + 1]   = {0, 0b10,  0b10,  0b10,  0b10,  0b10,  0b10,  0b01,  0b10 };

//Functions

// Any other functions used

//Called from the main loop every 10ms
void update_outputs(void){
    unsigned char m, s, p;
    unsigned char wait;

    //Invalid state number - show all red
    if((current_state < S1) || (current_state > S8)){
        all_red();
        return;
    }

    //Look up the outputs for the present state
    m = main_rag[current_state];
    s = side_rag[current_state];
    p = ped_rg[current_state];

    //Safety check - never allow two greens that conflict
    if( ((m & GREEN_BIT_3) && (s & GREEN_BIT_3)) ||   //main and side green together
        ((m & GREEN_BIT_3) && (p & GREEN_BIT_2)) ||   //main and pedestrians green together
        ((s & GREEN_BIT_3) && (p & GREEN_BIT_2)) ){   //side and pedestrians green together
        all_red();
        return;
    }

    //Main road
    MAIN_RED   = (m & RED_BIT_3)   ? 1 : 0;
    MAIN_AMBER = (m & AMBER_BIT_3) ? 1 : 0;
    MAIN_GREEN = (m & GREEN_BIT_3) ? 1 : 0;

    //Side road
    SIDE_RED   = (s & RED_BIT_3)   ? 1 : 0;
    SIDE_AMBER = (s & AMBER_BIT_3) ? 1 : 0;
    SIDE_GREEN = (s & GREEN_BIT_3) ? 1 : 0;

    //Pedestrian crossing (three pins each, all driven with the same signal)
    PED_RED_1   = (p & RED_BIT_2)   ? 1 : 0;
    PED_RED_2   = (p & RED_BIT_2)   ? 1 : 0;
    PED_RED_3   = (p & RED_BIT_2)   ? 1 : 0;
    PED_GREEN_1 = (p & GREEN_BIT_2) ? 1 : 0;
    PED_GREEN_2 = (p & GREEN_BIT_2) ? 1 : 0;
    PED_GREEN_3 = (p & GREEN_BIT_2) ? 1 : 0;

    //Wait lights - on while a crossing request is waiting to be served
    wait = ped_request ? 1 : 0;
    WAIT_LIGHT_1 = wait;
    WAIT_LIGHT_2 = wait;
    WAIT_LIGHT_3 = wait;
    WAIT_LIGHT_4 = wait;
}

//Puts every traffic and pedestrian light to red (used at power up and as a safety fallback)
void all_red(void){
    MAIN_RED = 1;  MAIN_AMBER = 0;  MAIN_GREEN = 0;
    SIDE_RED = 1;  SIDE_AMBER = 0;  SIDE_GREEN = 0;
    PED_RED_1 = 1;   PED_RED_2 = 1;   PED_RED_3 = 1;
    PED_GREEN_1 = 0; PED_GREEN_2 = 0; PED_GREEN_3 = 0;
}
