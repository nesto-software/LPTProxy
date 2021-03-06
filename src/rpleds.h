#ifndef _RPLEDS_H_
#define _RPLEDS_H_
#include <wiringPi.h>
    /* Library header to allow external access to the code for controlling the LEDs on the Retro-Printer module.
       v1.1 (c) RWAP Software
       v1.1 Update to use the WiringPi library
    */
       
    // Sets up the values for the GPIO pins and turns off all LEDs except for the power
    extern void initialize_leds();
    
    extern void access_leds(); // Get access to leds for secondary process
    
    // Functions to switch leds on or off - parameter is "blue", "red" or "green"
    extern void led_on(char *ledname);
    extern void led_off(char *ledname);
    
#endif
