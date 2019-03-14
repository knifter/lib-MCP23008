#include <Arduino.h>
#include <Wire.h>

#include <MCP23008.h>

#define PIN_SDA		23
#define PIN_SCL		22

MCP23008 mcp(Wire);

void setup()
{
	Serial.begin(115200);
	Serial.println("Start setup()");

	if(!Wire.begin(PIN_SDA, PIN_SCL, 400000))
	{
		Serial.println("Wire.begin() Failed.");
		while(1);
	}

	if(!mcp.begin())
	{
		Serial.print("Error = ");
		Serial.println(Wire.getErrorText(Wire.lastError()));
		while(1);
	};
	Serial.println("Found!");

	mcp.setPinMode(0, 	MCP23008::PINMODE_INPUT); 	// PWR_OK
	mcp.setPinMode(1, 	MCP23008::PINMODE_OUTPUT);	// 5V_USB_ENABLE
	mcp.setPinMode(2, 	MCP23008::PINMODE_INPUT);		// PWR_DLY_CHK
	mcp.setPinMode(3, 	MCP23008::PINMODE_OUTPUT);	// PWR_ENBL
	mcp.setPinMode(4, 	MCP23008::PINMODE_OUTPUT);	// 3V3A_ENBL
	mcp.setPinMode(5, 	MCP23008::PINMODE_OUTPUT);	// 3V3B_ENBL

	mcp.setPin(1, HIGH);
	mcp.setPin(3), HIGH);
	mcp.setPin(4, HIGH);
	mcp.setPin(5, LOW);
}

void dumpPort()
{
	uint8_t p = mcp.getPort();
	Serial.print("Port = ");
	Serial.println(p, BIN);
}

void loop()
{
	uint32_t t1, t2;

	Serial.println("ON");
	if(mcp.getPin(2))
		Serial.println("CHK = HIGH (ok)");
	else
		Serial.println("CHK = LOW (fail!)");
	dumpPort();
	mcp.setPin(1, HIGH);
	t1 = micros();
	while(mcp.getPin(2) == HIGH);
	t2 = micros();
	Serial.print("ON dly = ");
	Serial.println(t2 - t1, DEC);

	delay(1000);
	Serial.println("OFF");
	mcp.setPin(1, LOW);	
	t1 = micros();
	while(mcp.getPin(2) == LOW);
	t2 = micros();
	Serial.print("Off delay = ");
	Serial.println(t2 - t1, DEC);

	delay(1000);
	Serial.println("\n");
}
