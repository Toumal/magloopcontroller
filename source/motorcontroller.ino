/**
 * Magnetic Loop Remote Control
 * (c) 2014 Josef Jahn OE3JJS
 * 
 * Motor controller 
 * This program is designed to run on an Arduino Nano connected to a 
 * NRF24L01 wireless comms module and a H-bridge or similar motor controller. 
 *
 * Functions:
 * Listen to commands from remote transitter ("0", "1", "2")
 * Send endstop switch status to remote transmitter ("0", "1", "2") (also send them as response to any motor command)
 * Control the two motor outputs according to the remote commands.
 * Stop motor / don't allow reengagement if the appropriate endstop switch is engaged. 
 * 
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// Pin constants
const int motor1_pin =2;
const int motor2_pin = 3;
const int endstop1_pin = 4;
const int endstop2_pin = 5;

// Variables
int lastEndstopStatus = 0;
int currentMotorStatus = 0;

void setup(void)
{
        Serial.begin(57600);
        printf_begin();
        printf("\n\rRF24/magloop/motorcontroller/\n\r"); 

  
  	pinMode(motor1_pin, OUTPUT);
	pinMode(motor2_pin, OUTPUT);
	pinMode(endstop1_pin, INPUT);
	pinMode(endstop2_pin, INPUT);
	
	//Stop the motor
	digitalWrite(motor1_pin, LOW);
	digitalWrite(motor2_pin, LOW);
  
	digitalWrite(endstop1_pin, HIGH);
	digitalWrite(endstop2_pin, HIGH);
	delay(20); // Just to get a solid reading on the endstop pins

	// Setup and configure rf radio
	radio.begin();

	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15,15);

	// optionally, reduce the payload size.  seems to improve reliability
	radio.setPayloadSize(8);

        radio.setPALevel(RF24_PA_HIGH);

	// Open 'status' pipe for writing, open the 'command' pipe for reading, in position #1
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1,pipes[0]);

	// Start listening
	radio.startListening();
        radio.printDetails(); 
        radio.stopListening();
        delay(100);
	radio.startListening();
}

// Reads the current endstop switch state
int readEndstopSwitches() {
	int e1 = digitalRead(endstop1_pin);
	int e2 = digitalRead(endstop2_pin);
  
	if (e1 && e2)
		return 3;	// Both endstop switches hit? Something is wrong!
	else if (e2)
		return 2;   // Endstop switch 2
	else if (e1)
		return 1;   // Endstop switch 1

	return 0; // No endstop engaged
}

// Used to make the motor change direction or stop
void motorControl(int direction, int endstopStatus) {
        printf("motorControl: %i\n\r", direction); 
	if (direction == 0) {
		//Allow stopping the motor at any time
		digitalWrite(motor1_pin, LOW);
		digitalWrite(motor2_pin, LOW);
		currentMotorStatus = 0;
	}

	if (endstopStatus == 3) {
		//If endstop status is weird, stop the motor right there.
		digitalWrite(motor1_pin, LOW);
		digitalWrite(motor2_pin, LOW);
		currentMotorStatus = 0;
	} else if (direction == 1 && endstopStatus != 1) {
		digitalWrite(motor2_pin, LOW);
		digitalWrite(motor1_pin, HIGH);
		currentMotorStatus = 1;
	} else if (direction == 2 && endstopStatus != 2) {
		digitalWrite(motor1_pin, LOW);
		digitalWrite(motor2_pin, HIGH);
		currentMotorStatus = 2;
	}
}

// Transmit our endstop switch status. Usually used after it changes.
void transmitEndstopStatus(int status) {
    printf("transmitEndpointStatus: %i\n\r", status); 
    // Stop listening so we can send stuff
    radio.stopListening();
    bool ok = radio.write( &status, sizeof(int) );
    // Now, continue listening
    radio.startListening();
}

// Checks if we need to stop a currently running motor
void checkMotorDirectionEndstop(int endstopStatus) {
	if (currentMotorStatus == 1) {
		if (endstopStatus == 1 || endstopStatus == 3) {
                        printf("MOTOR STOP, endstop hit: %i\n\r", endstopStatus); 
			digitalWrite(motor1_pin, LOW);
			digitalWrite(motor2_pin, LOW);
			currentMotorStatus = 0;
		}
	} else if (currentMotorStatus == 2) {
		if (endstopStatus == 2 || endstopStatus == 3) {
                        printf("MOTOR STOP, endstop hit: %i\n\r", endstopStatus); 
			digitalWrite(motor1_pin, LOW);
			digitalWrite(motor2_pin, LOW);
			currentMotorStatus = 0;
		}
	}

}


void loop(void)
{
    int endstopStatus = readEndstopSwitches();

	if (endstopStatus != lastEndstopStatus) {
		transmitEndstopStatus(endstopStatus);
	}
	
	checkMotorDirectionEndstop(endstopStatus);
	
    if (radio.available()) {
      int remoteCommand;
      radio.read( &remoteCommand, sizeof(int) );
	  if (remoteCommand != currentMotorStatus) {
		motorControl(remoteCommand, endstopStatus);
	  }
 	  transmitEndstopStatus(endstopStatus);
   }
   
    lastEndstopStatus = endstopStatus;

    delay(20);
}
