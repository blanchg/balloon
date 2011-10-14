
#include "GPS.h"

GPS::GPS(int rxPin, int txPin) : SoftwareSerial(rxPin, txPin) 
{
}

GPS::~GPS()
{
  end();
}

void GPS::begin(int speed)
{
  SoftwareSerial::begin(speed);
  // setup gps unit to only output nmea string we want.
  //  GPGGA - Altitude
  //  GPRMC - Speed and Course

}

void GPS::end()
{
  SoftwareSerial::end();
}

void GPS::fixSpeed(int newSpeed)
{
  end();
  changeSpeed(4800, newSpeed);
  changeSpeed(9600, newSpeed);
  changeSpeed(19200, newSpeed);
  changeSpeed(38400, newSpeed);
  changeSpeed(57600, newSpeed);
  changeSpeed(115200, newSpeed);
  begin(newSpeed);
}

void GPS::changeSpeed(int currentSpeed, int newSpeed)
{
  SoftwareSerial::begin(currentSpeed);
  short rate = 0;
  switch(newSpeed) {
    case 4800:
      rate = 0;
      break;
    case 9600:
      rate = 1;
      break;
    case 19200:
      rate = 2;
      break;
    case 38400:
      rate = 3;
      break;
    case 57600:
      rate = 4;
      break;
    case 115200:
      rate = 5;
      break;
  }
  
  // Set it to new rate
  print(0xA0, BYTE);  // header
  print(0xA1, BYTE);
  print(0x00, BYTE);  // length
  print(0x04, BYTE);
  print(0x05, BYTE);  // message
  print(0x00, BYTE);  // COM 0 - on chip
  print(rate, BYTE);  // Rate
  print(0x01, BYTE);  // Temp = 0; Flash = 1
  print(0x05 ^ rate ^ 1, BYTE);  // cs
  print(0x0D, BYTE);  // footer
  print(0x0A, BYTE);
  SoftwareSerial::end();

}

void GPS::messageMode(short mode)
{
  // Set message mode
  print(0xA0, BYTE);  // header
  print(0xA1, BYTE);
  print(0x00, BYTE);  // length
  print(0x03, BYTE);
  print(0x09, BYTE);  // message
  print(mode, BYTE);  // type
  print(0x01, BYTE);  // temp = 0, flash = 1
  print(0x09 ^ mode ^ 1, BYTE);  // cs
  print(0x0D, BYTE);  // footer
  print(0x0A, BYTE);
}

void GPS::nmeaStrings()
{
    // Configure output of NMEA strings
  print(0xA0, BYTE); // header
  print(0xA1, BYTE);
  print(0x00, BYTE); // length
  print(0x09, BYTE); 
  print(0x08, BYTE); // message
  print(0x01, BYTE); // GGA  lat lon alt
  print(0x01, BYTE); // GSA
  print(0x01, BYTE); // GSV
  print(0x01, BYTE); // GLL
  print(0x01, BYTE); // RMC  speed and course
  print(0x01, BYTE); // VTG
  print(0x01, BYTE); // ZDA
  print(0x01, BYTE);// temp = 0, flash = 1
  print(0x08 ^ 1 ^ 0x1 ^ 0x1 ^ 0x1 ^ 1 ^ 0x1 ^ 0x1 ^ 1, BYTE);// cs
  print(0x0D, BYTE);// footer
  print(0x0A, BYTE);
}

void GPS::powerMode(bool powerSave)
{
    // Configure output of NMEA strings
  print(0xA0, BYTE); // header
  print(0xA1, BYTE);
  print(0x00, BYTE); // length
  print(0x03, BYTE); 
  print(0x0C, BYTE); // message
  print(powerSave, BYTE); // Power save = 1
  print(0x01, BYTE);// temp = 0, flash = 1
  print(0x0C ^ powerSave ^ 1, BYTE);// cs
  print(0x0D, BYTE);// footer
  print(0x0A, BYTE);
}


bool GPS::capture()
{
  // Start listening to gps
  listen();
  // Read from GPS
  bool have_gps = false;
  int attempts = 0;
  int max_attempts = 100;
  
  
  while (!have_gps)
  {
    while (!have_gps && available())
    {
      int c = read();
      Serial.write(c);
      if (data.encode(c))
      {
        // process new gps info here
        // returns +- latitude/longitude in hundred thousandths degrees and fix_age in millisecs
        data.get_position(&lat, &lon, &fix_age);
        
        if (fix_age != TinyGPS::GPS_INVALID_AGE)
        {          
          // date in ddmmyy, time in hhmmsscc
          data.get_datetime(&date, &time);
          alt = data.altitude();
          course = data.course();
          speed = data.speed();
          
          have_gps = true;
        }
      }
    }
    attempts++;
    if (attempts > max_attempts) {
      break;
    }
  }
  return have_gps;
}
