#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <GPS.h>

const int LED1PIN = 13;
const int BAUD = 9600;
GPS gps(4,5);

void setup()
{
  Serial.begin(BAUD);
  
  Serial.println("\r\nGPS Test");
  pinMode(LED1PIN, OUTPUT);
  digitalWrite(LED1PIN, HIGH);
  gps.fixSpeed(BAUD);
  gps.begin(BAUD);
  
  gps.nmeaStrings();
  gps.powerMode(false);
  
  Serial.println("Ready!");
  
  digitalWrite(LED1PIN, LOW);
}

void loop()
{
  /*if (gps.capture()) {
    Serial.print("$POS,");
    Serial.print(gps.lat);
    Serial.print(",");
    Serial.print(gps.lon);
    Serial.print(",");
    Serial.println(gps.fix_age);
  }*/
  while (gps.available()) {
    Serial.write(gps.read());
  }
  while (Serial.available()) {
    gps.write(Serial.read());
  }
}
