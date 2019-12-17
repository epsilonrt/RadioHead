// rf95_client.
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with NanoPi Core/Core2 with mini shield and LoRasPi breakout
// and Raspberry Pi 3 with LoRasPi breakout
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95
#ifdef __unix__
#include <Piduino.h>  // All the magic is here ;-)
#else
// Defines the serial port as the console on the Arduino platform
#define Console Serial
#endif

#include <SPI.h>
#include <RH_RF95.h>
#include <RHutil/RHGpioLed.h>

// Uncomment or complete the configuration below depending on what you are using
// ---------------------------
const float Frequency = 868.0;

// LoRasPi breakout leds (https://github.com/hallard/LoRasPI)
// if you do not have leds, you will also have to modify setup() accordingly
//RHGpioLed txLed (4); // TX/RX D3

// Singleton instance of the radio driver

RH_RF95 rf95;
//RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95

// NanoPi Core with mini shield, /dev/spidev1.0
// DIO0: GPIOA1, Ino pin 6,  CON1 pin 22
// CS  : GPIOA13, Ino pin 27,  CON1 pin 24
// RST : GPIOA3, Ino pin 3,  CON1 pin 15

//RH_RF95 rf95 (27, 6);

// Raspberry Pi, /dev/spidev0.0
// DIO0: GPIO25, Ino pin 6,  J8 pin 22
// CS  : GPIO8,  Ino pin 10, J8 pin 24
// RST : GPIO22, Ino pin 3,  J8 pin 15

//RH_RF95 rf95 (10, 6);

// ---------------------------
// End of configuration

void setup()
{
	Console.begin (115200);
	Console.println("rf95_client");

	//rf95.setTxLed (txLed);

	// Defaults after init are 434.0MHz, 13dBm,
	// Bw = 125 kHz, Cr = 5 (4/5), Sf = 7 (128chips/symbol), CRC on
	if (!rf95.init())
	{
		Console.println ("init failed");
		exit (EXIT_FAILURE);
	}

	// Setup ISM Frequency
	rf95.setFrequency (Frequency);

	// rf95.printRegisters (Console);
	Console.println("Press any key to send 'Hello World !' message....");
}

unsigned int counter;
// Dont put this on the stack:
char buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  // waits until a key is pressed....
	while (!Console.available())
		delay(10);
	Console.read(); // flush the key

	uint8_t len = sprintf (buf, "Hello World ! #%d", ++counter) + 1;

	Console.print ("S[");
	Console.print (len);
	Console.print ("]<");
	Console.print (buf);
	Console.print ("> waiting... ");

	// Send a message to rf95_server
	rf95.send((uint8_t *)buf, len);
	rf95.waitPacketSent();

	// Now wait for a reply
	if (rf95.waitAvailableTimeout(3000))
	{
		len = sizeof(buf);
		// Should be a reply message for us now
		if (rf95.recv((uint8_t *)buf, &len))
		{
			Console.print ("R[");
			Console.print (len);
			Console.print ("]<");
			Console.print (buf);
			Console.print ("> RSSI: ");
			Console.println (rf95.lastRssi(), DEC);
		}
	}
	else
	{
		Console.println ("no reply !");
	}
}
