// rf95_server
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_client
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
#include <RHutil/GpioLed.h>

using namespace std;

const float frequency = 868.0;

// Singleton instance of the radio driver
//RH_RF95 rf95;

// NanoPi Core with mini shield, /dev/spidev1.0
// DIO0: GPIOA1, Ino pin 6,  CON1 pin 22
// CS  : GPIOA13, Ino pin 27,  CON1 pin 24
// RST : GPIOA3, Ino pin 3,  CON1 pin 15
RH_RF95 rf95 (27, 6);

// Raspberry Pi, /dev/spidev0.0
// DIO0: GPIO25, Ino pin 6,  J8 pin 22
// CS  : GPIO8,  Ino pin 10, J8 pin 24
// RST : GPIO22, Ino pin 3,  J8 pin 15
//RH_RF95 rf95 (10, 6);

// LoRasPi breakout leds (https://github.com/hallard/LoRasPI)
GpioLed txLed (4); // TX/RX D3
GpioLed rxLed (5); // LED D4

void setup()
{
	Console.begin (115200);

	pinMode (3, OUTPUT);
	digitalWrite (3, HIGH);
	delay (1);
	digitalWrite (3, LOW);
	delay (10);
	pinMode (3, INPUT);

	rf95.setTxLed (txLed);
	rf95.setRxLed (rxLed);

	// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
	if (!rf95.init())
	{
		Console.println ("init failed");
		exit (EXIT_FAILURE);
	}

	// Setup ISM frequency
	rf95.setFrequency (frequency);
	// Setup Power,dBm
	rf95.setTxPower (13);

	// Setup Spreading Factor (6 ~ 12)
	rf95.setSpreadingFactor (7);

	// Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
	//Lower BandWidth for longer distance.
	rf95.setSignalBandwidth (125000);

	// Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8)
	rf95.setCodingRate4 (5);

	// rf95.printRegisters (Console);
	Console.println ("Waiting incoming messages....");
}

char buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

void loop()
{
	// Wait for a packet
	if (rf95.waitAvailableTimeout (3000))
	{
		len = sizeof(buf);
		// Should be a message for us now
		if (rf95.recv ((uint8_t *) buf, &len))
		{
			buf[len] = 0;
			Console.print ("R[");
			Console.print (len);
			Console.print ("]<");
			Console.print (buf);
			Console.print ("> RSSI: ");
			Console.print (rf95.lastRssi(), DEC);

			Console.print ("dBm, send reply > S[");
			Console.print (len);
			Console.print ("]<");
			Console.print (buf);
			Console.println (">");
			// Send a reply
			rf95.send ((uint8_t *)buf, len);
			rf95.waitPacketSent();
		}
	}
}
