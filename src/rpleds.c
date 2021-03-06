#include "rpleds.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/resource.h>

#include <unistd.h>

/* Library to allow external access to the code for controlling the LEDs on the Retro-Printer module.
   v1.1 (c) RWAP Software
   v1.1 Update to use the WiringPi library
*/

int ledpins[] =   { 6 ,10, 9};		// set online led, ack led, power led

int powerLed;
int onlineLed;
int ackLed;

int setup(void);

void initialize_leds()
{
    ackLed    = ledpins[0];
    onlineLed = ledpins[1];
    powerLed  = ledpins[2];

    pinMode(ledpins[0], INPUT); //SET ackLed
    pinMode(ledpins[0], OUTPUT); //SET ackLed
    pinMode(ledpins[1], INPUT); //SET onlineLed
    pinMode(ledpins[1], OUTPUT); //SET onlineLed
    pinMode(ledpins[2], INPUT); //SET powerLed
    pinMode(ledpins[2], OUTPUT); //SET powerLed

    digitalWrite (powerLed,  LOW);//blue
    digitalWrite (onlineLed, LOW);//red
    digitalWrite (ackLed,    LOW);//green
}

void access_leds()
{
    ackLed    = ledpins[0];
    onlineLed = ledpins[1];
    powerLed  = ledpins[2];

    pinMode(ledpins[0], INPUT); //SET ackLed
    pinMode(ledpins[0], OUTPUT); //SET ackLed
    pinMode(ledpins[1], INPUT); //SET onlineLed
    pinMode(ledpins[1], OUTPUT); //SET onlineLed
    pinMode(ledpins[2], INPUT); //SET powerLed
    pinMode(ledpins[2], OUTPUT); //SET powerLed

}

void led_off(char *ledname)
{
    if (strcmp(ledname,"red") == 0) {
    	digitalWrite (onlineLed, LOW);
    } else if (strcmp(ledname,"green") == 0) {
    	digitalWrite (ackLed, LOW);
    } else if (strcmp(ledname,"blue") == 0) {
    	digitalWrite (powerLed, LOW);
    }
}

void led_on(char *ledname)
{
    if (strcmp(ledname,"red") == 0) {
        digitalWrite (onlineLed, HIGH);
    } else if (strcmp(ledname,"green") == 0) {
    	digitalWrite (ackLed, HIGH);
    } else if (strcmp(ledname,"blue") == 0) {
    	digitalWrite (powerLed, HIGH);
    }
}
