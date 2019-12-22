// router_server
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, routed reliable messaging server
// with the RHRouter class.
// It is designed to work with the other example router_client.
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
#include <RHRouter.h>
#include <RHGpioPin.h>

// In this small artifical network of 3 nodes,
// messages are routed via intermediate nodes to their destination
// node. All nodes can act as routers
// Node1 <-> Node2 <-> Node3
const uint8_t Node1 = 1;
const uint8_t Node2 = 2;
const uint8_t Node3 = 3;


// Uncomment or complete the configuration below depending on what you are using
// ---------------------------
const uint8_t MyAddress = Node2; // <-- Change that ! perhaps ?
const float Frequency = 868.0;
const int8_t TxPower = 14;

// Singleton instance of the radio driver

//RH_RF95 driver;
//RH_RF95 driver(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 driver(8, 3); // Adafruit Feather M0 with RFM95

// NanoPi Core with mini shield, /dev/spidev1.0
// DIO0: GPIOA1, Ino pin 6,  CON1 pin 22
// CS  : GPIOA13, Ino pin 27,  CON1 pin 24
// RST : GPIOA3, Ino pin 3,  CON1 pin 15

RH_RF95 driver (27, 6);

// Raspberry Pi, /dev/spidev0.0
// DIO0: GPIO25, Ino pin 6,  J8 pin 22
// CS  : GPIO8,  Ino pin 10, J8 pin 24
// RST : GPIO22, Ino pin 3,  J8 pin 15

//RH_RF95 driver (10, 6);

// ---------------------------
// End of configuration

// Class to manage message delivery and receipt, using the driver declared above
RHRouter manager (driver, MyAddress);

RHGpioPin txLed (LedPin);

void setup()
{
	Console.begin (115200);
	Console.print ("router_server @");
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
	driver.setTxPower (TxPower);
	//driver.printRegisters (Console);

	// Node1 <-> Node2 <-> Node3
	switch (MyAddress)
	{
	case Node1:
		manager.addRouteTo (Node2, Node2);
		manager.addRouteTo (Node3, Node2);
		break;
	case Node2:
		manager.addRouteTo (Node1, Node1);
		manager.addRouteTo (Node3, Node3);
		break;
	case Node3:
		manager.addRouteTo (Node2, Node2);
		manager.addRouteTo (Node1, Node2);
		break;
	default:
		Console.println ("no valid address defined");
		exit (EXIT_FAILURE);
		break;
	}
	//manager.printRoutingTable(Console);

	Console.println ("Waiting for incoming messages....");
}

// Dont put this on the stack:
char buf[RH_ROUTER_MAX_MESSAGE_LEN];

void loop()
{
	if (manager.waitAvailableTimeout (3000))
	{
		// Wait for a message addressed to us from the client
		uint8_t len = sizeof(buf);
		uint8_t from;
		if (manager.recvfromAck((uint8_t *)buf, &len, &from))
		{
			Console.print ("from @");
			Console.print (from, DEC);
			Console.print (" R[");
			Console.print (len);
			Console.print ("]<");
			Console.print (buf);
			Console.print (">(");
			Console.print (driver.lastRssi(), DEC);
			Console.print ("dBm) ---> send reply to @");
			Console.print (from, DEC);
			Console.print (" ---> ");
			Console.flush();

			// Send a reply back to the originator client
			if (manager.sendtoWait ( (uint8_t *) buf, len, from) == RH_ROUTER_ERROR_NONE)
			{
				Console.print ("S[");
				Console.print (len);
				Console.print ("]<");
				Console.print (buf);
				Console.println (">");
			}
			else
				Console.println("sendtoWait failed");
		}
	}
}
