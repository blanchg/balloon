

#include "Temp.h"


#include "WConstants.h"
#include <Wire.h>

// Connections:
// GND - GND
// SDA - A4
// SCL - A5
// 3.3v - 3v3

// From the datasheet the BMP module address LSB distinguishes
// between read (1) and write (0) operations, corresponding to 
// address 0x91 (read) and 0x90 (write).
// shift the address 1 bit right (0x91 or 0x90), the Wire library only needs the 7
// most significant bits for the address 0x91 >> 1 = 0x48
// 0x90 >> 1 = 0x48 (72)

Temp::Temp(int address) :
	sensorAddress(address)
{
	Wire.begin();
}

Temp::~Temp()
{
}

void Temp::capture()
{
  int retries = 0;
  const int max_retries = 10;
  boolean found = false;
  while (!found && max_retries < 10) {
    retries++;
    Wire.requestFrom(sensorAddress,2);  // Request 2 bytes
    if (2 <= Wire.available())  // if two bytes were received 
    {
      msb = Wire.receive();  // receive high byte (full degrees)
      lsb = Wire.receive();  // receive low byte (fraction degrees) 
      temperature = ((msb) << 4);  // MSB
      temperature |= (lsb >> 4);   // LSB
      found = true;
      return;
    }
  }
  temperature = INVALID_TEMP;
}

float Temp::convertCelcius(int temperature)
{
  if (temperature == INVALID_TEMP)
  {
    return INVALID_TEMP;
  }
  else if (bitRead(temperature, 11))
  {
    // Negative temperature stored as twos complement
    return ((~(temperature) & 2047) -1) * 0.0625;
  }
  else
  {
    // Positive temperature
    return temperature*0.0625;
  }
}
