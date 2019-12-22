// rf95_encrypted_server
// Example sketch showing how to create a encrypted messaging server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_encrypted_client
// Tested with Arduino, NanoPi Core/Core2 with mini shield and LoRasPi breakout
// and Raspberry Pi 3 with LoRasPi breakout
//
// In order for this to compile on Arduino, you MUST uncomment the
// #define RH_ENABLE_ENCRYPTION_MODULE
// at the bottom of RadioHead.h, AND you MUST have installed the Crypto
// directory from arduinolibs: http://rweather.github.io/arduinolibs/index.html
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
#include <RHEncryptedDriver.h>
#include <AES.h>
#include <RHGpioPin.h>

// Uncomment or complete the configuration below depending on what you are using
// ---------------------------
const float frequency = 868.0;
const uint8_t encryptkey[16]= {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; // The very secret key !

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

AES256 cipher;   // Instanciate a AES256 block ciphering
RHEncryptedDriver driver(rf95, cipher); // Instantiate the driver with those two
RHGpioPin txLed (LedPin);

void setup()
{
	Console.begin (115200);
	Console.println("rf95_encrypted_server");

	rf95.setTxLed (txLed);

	// Defaults after init are 434.0MHz, 13dBm,
	// Bw = 125 kHz, Cr = 5 (4/5), Sf = 7 (128chips/symbol), CRC on
	if (!rf95.init())
	{
		Console.println ("init failed");
		exit (EXIT_FAILURE);
	}

	// Setup ISM frequency
	rf95.setFrequency (frequency);

	// rf95.printRegisters (Console);
	cipher.setKey(encryptkey, 16);
	Console.println ("Waiting for incoming messages....");
}

// Dont put this on the stack:
char buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
	// Wait for a packet
	if (driver.waitAvailableTimeout (3000))
	{
		uint8_t len = sizeof (buf);
		// Should be a message for us now
		if (driver.recv ( (uint8_t *) buf, &len))
		{
			Console.print ("R[");
			Console.print (len);
			Console.print ("]<");
			Console.print (buf);
			Console.print ("> RSSI: ");
			Console.print (driver.lastRssi(), DEC);

			Console.print ("dBm, send reply > S[");
			Console.print (len);
			Console.print ("]<");
			Console.print (buf);
			Console.println (">");
			// Send a reply
			driver.send ( (uint8_t *) buf, len);
			driver.waitPacketSent();
		}
	}
}
