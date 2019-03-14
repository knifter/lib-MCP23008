/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please supPin Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include "Arduino.h"
#include <TwoWireDevice.h>

#include "MCP23008.h"

#define REG_IODIR			0x00			// IO7..0 Direction, set to 1 for input
	#define IODIR_IN		0x01	
#define REG_IPOL			0x01			// IO7..0 Polarity, set to 1 to invert output
#define REG_GPINTEN			0x02			// IO7..0 Interrupt enabled = 1
#define REG_DEFVAL			0x03			// IO7..0 Default value, pinstate != defval => interrupt
#define REG_INTCON			0x04			// IO7..0 Inter. Control: 1 = DefVal reg, 0 = On-change
#define REG_IOCON			0x05			// Configuration Register
	#define IOCON_SEQOP		0x02			// Sequential I2C operation disabled (inc addr. ptr)
	#define IOCON_DISSLW	0x01			// Slew Rate control disabled on SDA pin
	#define IOCON_HAEN		0x08			// Hardware Address enable (MCP23S08 only)
	#define IOCON_ODR		0x04 			// INT pin Open-Drain Output enable
	#define IOCON_INTPOL	0x02			// INT pin polarity
#define REG_GPPU			0x06			// IO7..0 Pull-Up enabled (input only)
#define REG_INTF			0x07			// IO7..0 Interrupt flag
#define REG_INTCAP			0x08			// IO7..0 Interrupt Capture, Full GPIO Pin state when interrupt occured
#define REG_GPIO			0x09			// IO7..0 Read Pin status, Write Ouput Latch values
#define REG_OLAT			0x0A			// Output Latch register (set pins set as output)

bool MCP23008::begin()
{
	TwoWireDevice::begin();

	// Testread
	_reg_IODIR = readreg8(REG_IODIR);
	_reg_GPPU = readreg8(REG_GPPU);
	_reg_OLAT = readreg8(REG_OLAT);

	return (_wire.lastError() == 0);
};

void MCP23008::configIntPin(bool open_drain, bool invert)
{
	writereg8(REG_IOCON, (open_drain ? IOCON_ODR : 0) | (invert ? IOCON_INTPOL : 0));
};

/**
 * @brief Sets the pinmode per pin
 * 
 * POR = 1111 1111
 * When the bit is clear, the corresponding pin becomes an output.
 */
void MCP23008::setPinMode(const uint8_t pin, const PinMode_t mode)
{
	if(pin > 7)
		return;
	uint8_t mask = 1 << pin;
	switch(mode)
	{
		case PINMODE_INPUT_PULLUP:
			_reg_GPPU |= mask;
			_reg_IODIR |= mask;
			break;
		case PINMODE_INPUT:
			_reg_IODIR |= mask;
			_reg_GPPU &= ~mask;
			break;
		case PINMODE_OUTPUT:
			_reg_IODIR &= ~mask;
			break;
	};

	writereg8(REG_IODIR, _reg_IODIR);
	writereg8(REG_GPPU, _reg_GPPU);
};

void MCP23008::setIntMode(const uint8_t pin, const InterruptMode_t mode)
{
	if(pin > 7)
		return;
	uint8_t mask = 1 << pin;
	switch(mode)
	{
		case INTMODE_NONE:
			// Disable Int Enable
			_reg_GPINTEN &= ~mask;
			break;
		case INTMODE_LOW:
			// Set DEFVAL = 0 and interrupt on !defval, enable int
			_reg_DEFVAL &= ~mask;
			_reg_INTCON |= mask;
			_reg_GPINTEN |= mask;
			break;
		case INTMODE_HIGH:
			// Set DEFVAL = 1 and interrupt on !defval, enable int
			_reg_DEFVAL |= mask;
			_reg_INTCON |= mask;
			_reg_GPINTEN |= mask;
			break;		
		case INTMODE_CHANGE:
			// Int on change, enable int
			_reg_INTCON &= ~mask;
			_reg_GPINTEN |= mask;
			break;
	};

	writereg8(REG_INTCON, _reg_INTCON);
	writereg8(REG_DEFVAL, _reg_DEFVAL);
	writereg8(REG_GPINTEN, _reg_GPINTEN);
}

void MCP23008::setPort(const uint8_t port)
{
	writereg8(REG_OLAT, _reg_OLAT = port);
}

uint8_t MCP23008::getPort()
{
	return readreg8(REG_GPIO);
}

uint8_t MCP23008::getIntCap()
{
	return readreg8(REG_INTCAP);
}

void MCP23008::setPin(const uint8_t pin, const bool s)
{
	if(s)
		_reg_OLAT |= 1 << pin;
	else
		_reg_OLAT &= ~(1 << pin);
	writereg8(REG_OLAT, _reg_OLAT);
}

bool MCP23008::getPin(const uint8_t pin)
{
	uint8_t port = readreg8(REG_GPIO);
	return port & (1<<pin);
};
