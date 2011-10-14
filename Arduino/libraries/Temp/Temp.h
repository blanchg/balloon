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

#ifndef Temp_h
#define Temp_h


const int INVALID_TEMP = -999;


class Temp
{
private:
	short msb;
	short lsb;
public:
	int sensorAddress;
	int temperature;
	Temp(int sensorAddress);
	~Temp();
	void capture();
        float convertCelcius(int temperature);
};

#endif
