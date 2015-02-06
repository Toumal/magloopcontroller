/**
 * Magnetic Loop Remote Control
 * (c) 2014 Josef Jahn OE3JJS
 *
 * Remote Transmitter
 * This program is designed to run on an Arduino Mega connected to a 
 * NRF24L01 wireless comms module. 
 *
 * Functions:
 * Listen to status messages from motor controller ("0", "1", "2")
 * Send button status to motor controller ("0", "1", "2")
 * Control the LED outputs
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
const int led1_pin = 39;
const int led2_pin = 35;
const int ledlink_pin = 43;
const int button1_pin = 22;
const int button2_pin = 23;

int lastButtonStatus = 0;
int lastRemoteStatus = 0;
int recheckDelay = 0;
boolean dataReceived = 0;

void setup(void)
{
   Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/magloop/transmitter/\n\r"); 
 
  	pinMode(led1_pin, OUTPUT);
	pinMode(led2_pin, OUTPUT);
	pinMode(ledlink_pin, OUTPUT);

	digitalWrite(led1_pin,LOW);
	digitalWrite(led2_pin,LOW);
	digitalWrite(ledlink_pin,LOW);

	pinMode(button1_pin, INPUT);
	pinMode(button2_pin, INPUT);
	digitalWrite(button1_pin,HIGH);
	digitalWrite(button2_pin,HIGH);
	delay(20);

	// Setup and configure rf radio
	radio.begin();

	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15,15);
	
	// optionally, reduce the payload size.  seems to improve reliability
	radio.setPayloadSize(8);

        radio.setPALevel(RF24_PA_HIGH);

	// Open the 'command' pipe for sending, and the 'status' pipe for reading in position #1
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1,pipes[1]);

	// Start listening
	radio.startListening();
        radio.printDetails(); 
        radio.stopListening();
        delay(100);
	radio.startListening();
}

// Read the currently pressed button(s)
int readButtonStatus() {
	int b1 = digitalRead(button1_pin);
	int b2 = digitalRead(button2_pin);
	if (b1 && !b2)
		return 1;
	else if (!b1 && b2)
		return 2;

	return 0;
}

void transmitCommand(int status) {
    printf("Transmitting: %i\n\r", status); 
    // Stop listening so we can send stuff
    radio.stopListening();
    bool ok = radio.write( &status, sizeof(int) );
    // Now, continue listening
    radio.startListening();
}

void loop(void)
{
	int buttons = readButtonStatus();
	// If the button states changed, send that to the motor controller
	if (buttons != lastButtonStatus) {
		transmitCommand(buttons);
		lastButtonStatus = buttons;
	}

	// Display endstop status on the LEDs
    dataReceived = false;
    if (radio.available()) {
      dataReceived = true;
      int remoteStatus;
      radio.read( &remoteStatus, sizeof(int) );
		if (remoteStatus != lastRemoteStatus) {
                        printf("Remote Endstop Status: %i\n\r", remoteStatus); 
			if (remoteStatus == 1) {
				digitalWrite(led1_pin, HIGH);
				digitalWrite(led2_pin, LOW);
			} else if (remoteStatus == 2) {
				digitalWrite(led1_pin, LOW);
				digitalWrite(led2_pin, HIGH);
			} else {
				digitalWrite(led1_pin, LOW);
				digitalWrite(led2_pin, LOW);
			}
		}
		lastRemoteStatus = remoteStatus;
    }

	// Activate/Deactivate link status LED
	if (dataReceived) {
		digitalWrite(ledlink_pin, HIGH);
                recheckDelay=10;
	} else if (recheckDelay <= 0) {
		digitalWrite(ledlink_pin, LOW);
	} else {
                if (recheckDelay > 0)
                        recheckDelay--;
        }
	
    delay(20);


}
