// RHSPIDriver.cpp
//
// Copyright (C) 2014 Mike McCauley
// $Id: RHSPIDriver.cpp,v 1.11 2017/11/06 00:04:08 mikem Exp $

#include <RHSPIDriver.h>

RHSPIDriver::RHSPIDriver (uint8_t slaveSelectPin, RHGenericSPI& spi)
	:
	_spi (spi),
	_slaveSelectPin (slaveSelectPin)
{
}

bool RHSPIDriver::init()
{
	// start the SPI library with the default speeds etc:
	// On Arduino Due this defaults to SPI1 on the central group of 6 SPI pins
	_spi.begin();

	// Initialise the slave select pin
	// On Maple, this must be _after_ spi.begin
	pinMode (_slaveSelectPin, OUTPUT);
	digitalWrite (_slaveSelectPin, HIGH);

	delay (100);
	return true;
}

uint8_t RHSPIDriver::spiRead (uint8_t reg)
{
	uint8_t buf[2];
	buf[0] = reg & ~RH_SPI_WRITE_MASK;
	buf[1] = 0;
	ATOMIC_BLOCK_START;
	_spi.beginTransaction();
	digitalWrite (_slaveSelectPin, LOW);
	_spi.transfer (buf, 2); // Send the address with the write mask on read the value
	digitalWrite (_slaveSelectPin, HIGH);
	_spi.endTransaction();
	ATOMIC_BLOCK_END;
	return buf[1];
}

uint8_t RHSPIDriver::spiWrite (uint8_t reg, uint8_t val)
{
	uint8_t buf[2];
	buf[0] = reg | RH_SPI_WRITE_MASK;
	buf[1] = val;
	ATOMIC_BLOCK_START;
	_spi.beginTransaction();
	digitalWrite (_slaveSelectPin, LOW);
	_spi.transfer (buf, 2); // Send the address with the write mask on and value
	digitalWrite (_slaveSelectPin, HIGH);
	_spi.endTransaction();
	ATOMIC_BLOCK_END;
	return buf[0];
}

uint8_t RHSPIDriver::spiBurstRead (uint8_t reg, uint8_t* dest, uint8_t len)
{
#if (RH_PLATFORM != RH_PLATFORM_PIDUINO)
	uint8_t status = 0;
	ATOMIC_BLOCK_START;
	_spi.beginTransaction();
	digitalWrite (_slaveSelectPin, LOW);
	status = _spi.transfer (reg & ~RH_SPI_WRITE_MASK); // Send the start address with the write mask off
	while (len--)
		*dest++ = _spi.transfer (0);
	digitalWrite (_slaveSelectPin, HIGH);
	_spi.endTransaction();
	ATOMIC_BLOCK_END;
	return status;
#else
	uint8_t * buf = new uint8_t[len + 1];
	buf[0] = reg & ~RH_SPI_WRITE_MASK;
	_spi.beginTransaction();
	digitalWrite (_slaveSelectPin, LOW);
	_spi.transfer (buf, len + 1);
	digitalWrite (_slaveSelectPin, HIGH);
	_spi.endTransaction();
	memcpy (dest, &buf[1], len);
	reg = buf[0];
	delete[] buf;
	return reg;
#endif
}

uint8_t RHSPIDriver::spiBurstWrite (uint8_t reg, const uint8_t* src, uint8_t len)
{
#if (RH_PLATFORM != RH_PLATFORM_PIDUINO)
	uint8_t status = 0;
	ATOMIC_BLOCK_START;
	_spi.beginTransaction();
	digitalWrite (_slaveSelectPin, LOW);
	status = _spi.transfer (reg | RH_SPI_WRITE_MASK); // Send the start address with the write mask on
	while (len--)
		_spi.transfer (*src++);
	digitalWrite (_slaveSelectPin, HIGH);
	_spi.endTransaction();
	ATOMIC_BLOCK_END;
	return status;
#else
	uint8_t * buf = new uint8_t[len + 1];
	buf[0] = reg | RH_SPI_WRITE_MASK;
	memcpy (&buf[1], src, len);
	_spi.beginTransaction();
	digitalWrite (_slaveSelectPin, LOW);
	_spi.transfer (buf, len + 1);
	digitalWrite (_slaveSelectPin, HIGH);
	_spi.endTransaction();
	reg = buf[0];
	delete[] buf;
	return reg;
#endif
}

void RHSPIDriver::setSlaveSelectPin (uint8_t slaveSelectPin)
{
	_slaveSelectPin = slaveSelectPin;
}

void RHSPIDriver::spiUsingInterrupt (uint8_t interruptNumber)
{
	_spi.usingInterrupt (interruptNumber);
}
