// rf95_reliable_datagram_client
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_RF95 driver to control a RF95 radio.
// It is designed to work with the other example rf95_reliable_datagram_server
// Tested with Arduino, NanoPi Core/Core2 with mini shield and LoRasPi breakout
// and Raspberry Pi 3 with LoRasPi breakout
#ifdef __unix__
#include <Piduino.h>  // All the magic is here ;-)
// LoRasPi breakout TX/RX D3 led (https://github.com/hallard/LoRasPI)
const int LedPin = 4;
#else
// Defines the serial port as the console on the Arduino platform
#define Console Serial
const int LedPin = LED_BUILTIN;
#endif

#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <RHGpioPin.h>

// In this small artifical network of 3 nodes,
// Node1 <-> Node2 <-> Node3
const uint8_t Node1 = 1;
const uint8_t Node2 = 2;
const uint8_t Node3 = 3;

// Uncomment or complete the configuration below depending on what you are using
// ---------------------------
const uint8_t MyAddress = Node1; // <-- Change that ! perhaps ?
const float Frequency = 868.0;

// Singleton instance of the radio driver

RH_RF95 driver;
//RH_RF95 driver(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 driver(8, 3); // Adafruit Feather M0 with RFM95

// NanoPi Core with mini shield, /dev/spidev1.0
// DIO0: GPIOA1, Ino pin 6,  CON1 pin 22
// CS  : GPIOA13, Ino pin 27,  CON1 pin 24
// RST : GPIOA3, Ino pin 3,  CON1 pin 15

//RH_RF95 driver (27, 6);

// Raspberry Pi, /dev/spidev0.0
// DIO0: GPIO25, Ino pin 6,  J8 pin 22
// CS  : GPIO8,  Ino pin 10, J8 pin 24
// RST : GPIO22, Ino pin 3,  J8 pin 15

//RH_RF95 driver (10, 6);

// ---------------------------
// End of configuration

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, MyAddress);
RHGpioPin txLed (LedPin);

void setup()
{
	Console.begin (115200);
	Console.print("reliable_datagram_client @");
	Console.println (MyAddress, DEC);

	driver.setTxLed (txLed);

	// Defaults after init are 434.0MHz, 13dBm,
	// Bw = 125 kHz, Cr = 5 (4/5), Sf = 7 (128chips/symbol), CRC on
	if (!manager.init())
	{
		Console.println ("init failed");
		exit (EXIT_FAILURE);
	}

	// Setup ISM Frequency
	driver.setFrequency (Frequency);

	// driver.printRegisters (Console);
	Console.println ("Press 2 or 3 to send 'Hello World!' message to server 2 or 3....");
}

unsigned int counter;
// Dont put this on the stack:
char buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
	char dest;

	// waits until a key is pressed....
	while (!Console.available())
		delay(20);
	dest = Console.read() - '0'; // ASCII {'0'..'9'} -> BIN {0..9}

	if (dest == Node2 || dest == Node3)
	{
		uint8_t len = sprintf (buf, "Hello World ! #%d", ++counter) + 1;

		Console.print ("S[");
		Console.print (len);
		Console.print ("]<");
		Console.print (buf);
		Console.print (">--->@");
		Console.print (dest, DEC);
		Console.flush();

		// Send a message to manager_server
		if (manager.sendtoWait((uint8_t *)buf, len, dest))
		{
			// Now wait for a reply from the server
			len = sizeof(buf);
			uint8_t from;

			Console.print (" awaiting response ---");
			Console.flush();
			if (manager.recvfromAckTimeout((uint8_t *)buf, &len, 2000, &from))
			{
				Console.print (">R[");
				Console.print (len);
				Console.print ("]<");
				Console.print (buf);
				Console.print (">(");
				Console.print (driver.lastRssi(), DEC);
				Console.print ("dBm) from @");
				Console.println (from, DEC);
			}
			else
			{
				Console.println ("> no reply !");
			}
		}
		else
			Console.println(" Unable to deliver to host ! Checks if host is up.");
	}
}
