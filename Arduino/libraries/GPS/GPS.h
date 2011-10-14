

#ifndef gps_h
#define gps_h

#include <SoftwareSerial.h>
#include <TinyGPS.h>

const short GPS_MSG_NONE = 0;
const short GPS_MSG_NMEA = 1;
const short GPS_MSG_BIN = 2;

class GPS : public SoftwareSerial
{
public:
  long lat;  // 100,000ths of a degree
  long lon;  // 100,000ths of a degree
  long alt;  // cm
  unsigned long date;
  unsigned long time;
  unsigned long fix_age;
  unsigned long course;  // 100ths of a degree
  unsigned long speed;  // 100ths of a knot  * 1.852 to get kph
  TinyGPS data;
  GPS(int rxPin, int txPin);
  ~GPS();
  void begin(int speed);
  void end();
  bool capture();
  void fixSpeed(int newSpeed);
  void messageMode(short mode);
  void nmeaStrings();
  void powerMode(bool powerSave);
  void changeSpeed(int currentSpeed, int newSpeed);
};



#endif
