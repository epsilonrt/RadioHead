// rf95_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_RF95 driver to control a RF95 radio.
// It is designed to work with the other example rf95_reliable_datagram_server
// Tested with NanoPi Core/Core2 with mini shield and LoRasPi breakout
// and Raspberry Pi 3 with LoRasPi breakout
#ifdef __unix__
#include <Piduino.h>  // All the magic is here ;-)
#else
// Defines the serial port as the console on the Arduino platform
#define Console Serial
#endif

#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <RHutil/RHGpioLed.h>

// Uncomment or complete the configuration below depending on what you are using
// ---------------------------
const uint8_t ClientAddress = 1;
const uint8_t ServerAddress = 2;
const float Frequency = 868.0;

// LoRasPi breakout leds (https://github.com/hallard/LoRasPI)
// if you do not have leds, you will also have to modify setup() accordingly
RHGpioLed txLed (4); // TX/RX D3
RHGpioLed rxLed (5); // LED D4

// Singleton instance of the radio driver

//RH_RF95 driver;
//RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95

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
RHReliableDatagram manager(driver, ClientAddress);

void setup()
{
	Console.begin (115200);

	driver.setTxLed (txLed);
	driver.setRxLed (rxLed);

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
	Console.println ("Waiting for incoming messages....");
}

uint8_t data[] = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
	Console.println("Sending to rf95_reliable_datagram_server");

	// Send a message to manager_server
	if (manager.sendtoWait(data, sizeof(data), ServerAddress))
	{
		// Now wait for a reply from the server
		uint8_t len = sizeof(buf);
		uint8_t from;
		if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
		{
			Console.print("got reply from : 0x");
			Console.print(from, HEX);
			Console.print(": ");
			Console.println((char*)buf);
		}
		else
		{
			Console.println("No reply, is rf95_reliable_datagram_server running?");
		}
	}
	else
		Console.println("sendtoWait failed");
	delay(500);
}
