// rf24_reliable_datagram_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging server
// with the RHReliableDatagram class, using the RH_RF24 driver to control a RF24 radio.
// It is designed to work with the other example rf24_reliable_datagram_client
// Tested on Anarduino Mini http://www.anarduino.com/mini/ with RFM24W and RFM26W

#include <RHReliableDatagram.h>
#include <RH_RF24.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF24 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup()
{
	Serial.begin(9600);
	if (!manager.init())
		Serial.println("init failed");
	// The default radio config is for 30MHz Xtal, 434MHz base freq 2GFSK 5kbps 10kHz deviation
	// power setting 0x10
	// If you want a different frequency mand or modulation scheme, you must generate a new
	// radio config file as per the RH_RF24 module documentation and recompile
	// You can change a few other things programatically:
	//driver.setFrequency(435.0); // Only within the same frequency band
	//driver.setTxPower(0x7f);
}

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_RF24_MAX_MESSAGE_LEN];

void loop()
{
	if (manager.available())
	{
		// Wait for a message addressed to us from the client
		uint8_t len = sizeof(buf);
		uint8_t from;
		if (manager.recvfromAck(buf, &len, &from))
		{
			Serial.print("got request from : 0x");
			Serial.print(from, HEX);
			Serial.print(": ");
			Serial.println((char*)buf);

			// Send a reply back to the originator client
			if (!manager.sendtoWait(data, sizeof(data), from))
				Serial.println("sendtoWait failed");
		}
	}
}

