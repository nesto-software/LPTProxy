#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <error.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/resource.h>
#include <unistd.h>
#include <time.h> 
#include <SDL2/SDL.h>

#include <termios.h> 
#include <wiringPi.h>
#include "rpleds.h"

/* Retro-Printer Interface - Program by RWAP Software to read the data captured by the Retro-Printer Hardware
 * (read via the GPIO connector) and store it on RaspberryPi for processing
 * v1.0
 *
 * Based on v1.15 Retro-Printer Read Printer Interface
 * 
 */

int timeout = 4;            // printout finished after this time
int controlPowerLed;
int controlAckLed;
int controlOnlineLed;
int offlineStatus          = 0;
int offlineswitchControl   = 1;
int msbsetting             = 0;

int starttime;
struct timespec tvx;
int datablink=0;
int gpio_state = 1;
int processedCount = 0;
int bytesCaptured = 0;
int capturedData = 0;
int timeExpired = 0;

int page                = 1;
unsigned int signalTime = 5;        // Amount of time to retain the busy and ACK signals (msec)
int character_threshold = 2;        // Minimum number of characters to process
int ackposition         = 0;        // Configurable handshaking options from 0 to 4


// Bits to read
int bit_depth;              // Do we use all 8 bits or less?

int ipins[] =   { 8, 1,  7, 0 , 2 , 3 , 12, 13, 14, 11 };	// d0-7 last one
int opins[] = { 4, 5, 10};     // set busy, ack, reset busy

int busy;
int reset; //reset busy
int set;     //set busy
int ack;
int online;

int charsigot = 0;
struct sched_param x, xx;
struct timespec tv;
char filename[1000];
extern char path[];
char pathraw[1000];

extern struct termios options;
extern int sfd;
extern FILE *FPRINTER;
int sysResult;
FILE *fp;

void initialize()
{
    FILE *FL;
    int g, highestpage;
    char x[1000];
    char *config;

    sprintf(x,"mkdir %s 2>>/dev/null",pathraw);
    sysResult = system(x);

    wiringPiSetup();

    reset     = opins[0];
    ack       = opins[1];
    set       = opins[2];

    busy   = ipins[8];
    online = ipins[9];

    initialize_leds();

    // setup inputs
    for (g = 0; g <= 9; g++) {
        pinMode(ipins[g], OUTPUT);
        pinMode(ipins[g], INPUT);
        pullUpDnControl(ipins[g], PUD_UP);
    }
    pinMode (reset, INPUT);
    pinMode (reset, OUTPUT);

    pinMode (ack  , INPUT);
    pinMode (ack  , OUTPUT);
    pinMode (set  , INPUT);
    pinMode (set  , OUTPUT);

    digitalWrite (set, LOW);
  	usleep(500);
    digitalWrite (set, HIGH);

    digitalWrite (reset, LOW);
  	usleep(500);
    digitalWrite (reset, HIGH);

    digitalWrite (ack, HIGH);
    usleep(500);
    digitalWrite (ack, LOW);

    if (controlPowerLed) led_on("blue"); // Power LED as we are now ready to start receiving data
}

void set_busy_ack() //if (ackposition==0)
{
	digitalWrite(reset, LOW);
    usleep(5);
	digitalWrite(reset, HIGH);
}

void set_ack_busy_ack_busy() //if (ackposition==1)
{
	digitalWrite(ack, HIGH);
    usleep(signalTime);
    digitalWrite(reset, LOW);
    usleep(5);
}

void set_busy_ack_busy_ack() //if (ackposition==2)
{
	digitalWrite(reset, LOW);
    usleep(5);
    digitalWrite(ack, HIGH);
    usleep(signalTime);
}

void set_ack_busy() //if (ackposition==3)
{
	digitalWrite(ack, HIGH);
    usleep(signalTime);
    digitalWrite(ack, LOW);
    usleep(signalTime);
}

void set_busy_ack_ack_busy() //if (ackposition==4)
{
    digitalWrite(reset, LOW);
    digitalWrite(ack, HIGH);
    usleep(signalTime);
}

void reset_busy_ack(int ackTime) //if (ackposition==0)
{
	digitalWrite(ack, HIGH);
    usleep(signalTime);
    digitalWrite(ack, LOW);
}

void reset_ack_busy_ack_busy(int ackTime) //if (ackposition==1)
{
    digitalWrite(ack, LOW);
    if (ackTime > 1) usleep(ackTime);
    digitalWrite(reset, HIGH);
}

void reset_busy_ack_busy_ack(int ackTime) //if (ackposition==2)
{
    digitalWrite(reset, HIGH);
    if (ackTime > 1) usleep(ackTime);
    digitalWrite(ack, LOW);
}

void reset_ack_busy(int ackTime) //if (ackposition==3)
{
    digitalWrite(reset, LOW);
    usleep(5);
    digitalWrite(reset, HIGH);
}

void reset_busy_ack_ack_busy(int ackTime) //if (ackposition==4)
{
    digitalWrite(ack, LOW);
    digitalWrite(reset, HIGH);
}

int test_offline_switch(int offlineswitchControl)
{
    int g;
    if (offlineswitchControl) {
        //test if offline (online button not pressed)
        g=digitalRead(online);
        if (g) {
            return 1; // Offline
        } else {
            return 0;
        }
    }
    return 0;
}

int wait_until_online(int offlineswitchControl)
{
    int g;
    if (offlineswitchControl) {
        //test if offline (online button not pressed)
        g=digitalRead(online);
        if (g) {
            // printf("GPIO7 = %d ",digitalRead(online));
            // printf(" offline\n");
            if (controlOnlineLed) led_off("red"); // Online LED
            if (controlAckLed) led_off("green"); // Ack LED
            //if (controlPowerLed) pokePacketWord(200,1); // Signal the conversion program that it can control leds
            while (g) {
                // Wait for user to turn retro-printer back online
                usleep(1000*10);
                g=digitalRead(online);
            }
            // printf("GPIO7 = %d ",digitalRead(online));
            if (controlOnlineLed) led_on("red"); // Online LED
            // printf(" online\n");
        } else {
            if (controlOnlineLed) led_on("red"); // Online LED
        }
    }
    return 0;
}

int read_byte_from_gpio(unsigned char *bytex)
{
    int  i, offlineStatus;
    static int g=0;
    int altetime;
    struct timespec start, end;
    double ms_start, ms_end;
    int diff;

    unsigned static char databyte=0;

    clock_gettime(CLOCK_MONOTONIC, &tv);
    altetime = tv.tv_sec;

  gpio_wait:
    offlineStatus = test_offline_switch(offlineswitchControl);

    if (offlineStatus) {
        if (controlAckLed) led_off("green"); // Ack LED
        wait_until_online(offlineswitchControl);
    } else {
        g = digitalRead(busy);
        if (g) {
            switch (ackposition) {
                case 0:
                    set_busy_ack();
                    break;
                case 1:
                    set_ack_busy_ack_busy();
                    break;
                case 2:
                    set_busy_ack_busy_ack();
                    break;
                case 3:
                    set_ack_busy();
                    break;
                case 4:
                    set_busy_ack_ack_busy();
                    break;
            }
            if (controlAckLed) {
                if (datablink < 10) {
                    led_on("green");    //switch ack LED on after 10 characters were received
                } else {
                    led_off("green");   //switch ack LED for 5 characters off
                    if (datablink >= 15) datablink = 0;
                }
            }
            goto gpio_gogo;
        }
    }

    // printf("chars I got %d\n", charsigot);fflush(stdout);
    clock_gettime(CLOCK_MONOTONIC, &tv);
    timeExpired = tv.tv_sec - altetime;

    if (timeExpired > timeout) {
        altetime = tv.tv_sec;
        if (controlAckLed) led_off("green"); // Ack LED
        return 0;
    }

    if (timeExpired > 0) //switch off ack led if for more than 1 sec no data was received
    {
        if (controlAckLed) led_off("green"); // Ack LED
        datablink=0;
    }
    goto gpio_wait;

  gpio_gogo:
    clock_gettime(CLOCK_MONOTONIC, &start);
    ms_start = (double) start.tv_sec * 1000 + 1.0e-6*start.tv_nsec;
    databyte = 0;
    //if (controlPowerLed) pokePacketWord(200,2); // Signal the conversion program that capture program is controlling leds

    if (digitalRead(ipins[0]) > 0) databyte |= 1;
    if (digitalRead(ipins[1]) > 0) databyte |= 2;
    if (digitalRead(ipins[2]) > 0) databyte |= 4;
    if (digitalRead(ipins[3]) > 0) databyte |= 8;
    if (digitalRead(ipins[4]) > 0) databyte |= 16;
    if (digitalRead(ipins[5]) > 0) databyte |= 32;
    if (digitalRead(ipins[6]) > 0) databyte |= 64;

    switch (msbsetting) {
    case 0:
        if (digitalRead(ipins[7]) > 0) databyte |= 128;
        break;
    case 1:
        // MSB setting clears bit 7
        break;
    case 2:
        // MSB setting forces bit 7 to 1
        databyte |= 128;
        break;
    }
    charsigot++;
    if (controlAckLed) datablink++;
    *bytex = databyte;

    // Work out how long the signal has been on for as it needs to be a minimum of 5ms
    clock_gettime(CLOCK_MONOTONIC, &end);
    ms_end = (double) end.tv_sec * 1000 + 1.0e-6*end.tv_nsec;
    diff = (int) (ms_end - ms_start);

    switch (ackposition) {
        case 0:
            reset_busy_ack(signalTime - diff);
            break;
        case 1:
            reset_ack_busy_ack_busy(signalTime - diff);
            break;
        case 2:
            reset_busy_ack_busy_ack(signalTime - diff);
            break;
        case 3:
            reset_ack_busy(signalTime - diff);
            break;
        case 4:
            reset_busy_ack_ack_busy(signalTime - diff);
            break;
    }

    offlineStatus = test_offline_switch(offlineswitchControl);
    if (offlineStatus) {
        // We have processed waiting data and can now take the printer offline
        wait_until_online(offlineswitchControl);
    }

    return 1;
}

void raw_output(unsigned char xd)
{
    int flush_output;

    // Print out a character to the raw file
    if (fp == NULL) {
        // Create a raw file to stream captured data to
        sprintf(filename, "%s%d.raw", pathraw, page);
        fp = fopen(filename, "w");
    }
    fputc(xd, fp);
    fflush(fp);
    processedCount++;
}

int read_interface()
{
    unsigned char xd;
    unsigned char *packet;
    int c, g;
    int bytesRead = 0;
    int bytesCaptured = 0;

    strcpy(pathraw,"/home/pi/");
    strcat(pathraw,"raw/");

    //initialize_interface_comms();
    initialize();

    // Wait until the unit is switched online
    wait_until_online(offlineswitchControl);

  main_gpio_read_loop:
    
    xd = 0;
    gpio_state = 0;

    if (timeout > -1) {
        clock_gettime(CLOCK_MONOTONIC, &tvx);
        starttime = tvx.tv_sec;
    }
    while (gpio_state == 0) {
        gpio_state = read_byte_from_gpio((unsigned char *) &xd);  // byte1

        // Check to see if data not received within timeout span (-1 is never timeout)
        if (timeout > -1) {
            clock_gettime(CLOCK_MONOTONIC, &tvx);
            if (((tvx.tv_sec - starttime) >= timeout) && (gpio_state == 0)) goto exit_gpio_read_loop;
        }
    }
    capturedData = 1;
    if (bit_depth == 7) {
        // Set MSB (bit 7) of all incoming data to 0
        msbsetting = 1;
    }

    // Output the captured character
    raw_output(xd);
    bytesCaptured++;    

    goto main_gpio_read_loop;

  exit_gpio_read_loop:
    // Signify that end of print job has been reached    
    if (processedCount) {
        if (fp != NULL) {
            fflush(fp);
            fclose(fp);

            if (bytesCaptured >= character_threshold) {
                fp = NULL;
                page++;
            } else {
                // We have captured a tiny amount of data - below threshold, therefore it is an anomaly and can be deleted
                sprintf(filename, "unlink %s%d.raw", pathraw, page);                
                printf("command = %s \n", filename);
                sysResult = system(filename);
                fp = NULL;
                processedCount = 0;
            }
        }
    }
    
    bytesCaptured = 0;
    goto main_gpio_read_loop;
}
