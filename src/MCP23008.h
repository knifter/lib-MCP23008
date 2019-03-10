#ifndef __MCP23008_H
#define __MCP23008_H

#include <Arduino.h>
#include <TwoWireDevice.h>

#define MCP23008_ADDRESS_DEFAULT              (0x00)

class MCP23008: public TwoWireDevice {
    public:    
        typedef enum
        {
            PINMODE_INPUT = 0x01,
            PINMODE_OUTPUT = 0x02,
            PINMODE_INPUT_PULLUP = 0x05,
        } PinMode_t;

        typedef enum
        {
            INTMODE_NONE = 0xFF,
            INTMODE_LOW = 0x00,
            INTMODE_HIGH = 0x01,
            INTMODE_CHANGE = 0x03,
        } InterruptMode_t;

        MCP23008(TwoWire& wire, const uint8_t addr = MCP23008_ADDRESS_DEFAULT) : TwoWireDevice(wire, addr) {};
        MCP23008(const uint8_t addr = MCP23008_ADDRESS_DEFAULT) : TwoWireDevice(addr) {};

        bool 	begin();
        void    configIntPin(bool open_drain, bool invert);
        void    setPinMode(const uint8_t pin, const PinMode_t mode);
        void    setIntMode(const uint8_t pin, const InterruptMode_t mode);
        bool    getPin(const uint8_t pin);
        void    setPin(const uint8_t pin, bool s);
        uint8_t getPort();
        void    setPort(const uint8_t port);
        uint8_t getIntCap();

    protected:

    private:
        MCP23008(const MCP23008&);
        MCP23008& operator=(const MCP23008&);

        uint8_t _reg_GPPU;
        uint8_t _reg_IODIR;
        uint8_t _reg_OLAT;
        uint8_t _reg_INTCON = 0x00;
        uint8_t _reg_DEFVAL = 0x00;
        uint8_t _reg_GPINTEN = 0x00;    
};

#endif // __MCP23008_H
