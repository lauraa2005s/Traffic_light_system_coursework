/*
 * Filename: config.c
 * Author:
 * Date:
 * Revision Number: 1
 * Target Device: PIC18F452
 * Compiler: XC8
 * Summary: Configuration bit settings for the traffic light controller.
 *          These only need to appear in ONE file of the project.
*/

//Configuration statements
// CONFIG1H
#pragma config OSC = HS         // Oscillator Selection bits (HS oscillator)
#pragma config OSCS = OFF       // Oscillator System Clock Switch Enable bit (main oscillator is source)
// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bit (Brown-out Reset enabled)
#pragma config BORV = 20        // Brown-out Reset Voltage bits (VBOR set to 2.0V)
// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled)
#pragma config WDTPS = 128      // Watchdog Timer Postscale Select bits (1:128)
// CONFIG3H
#pragma config CCP2MUX = ON     // CCP2 Mux bit (CCP2 input/output is multiplexed with RC1)
// CONFIG4L
#pragma config STVR = ON        // Stack Full/Underflow Reset Enable bit (Stack Full/Underflow will cause RESET)
#pragma config LVP = OFF        // Low Voltage ICSP DISABLED - must be OFF because RB5 is a wait light output
                                // (with LVP on, RB5 becomes the PGM pin and cannot be used)
// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 not code protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 not code protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 not code protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 not code protected)
// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (not code protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (not code protected)
// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 not write protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 not write protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 not write protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 not write protected)
// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (not write protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (not write protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (not write protected)
// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 not protected)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 not protected)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 not protected)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 not protected)
// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (not protected)

// #pragma config statements should precede project file includes.

//Included Files
#include <xc.h>

/* Connection information
 * None - this file only holds the configuration bits.
*/
