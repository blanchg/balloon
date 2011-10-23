#include <TinyGPS.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "Temp.h"
#include "GPS.h"
#include <GPRS.h>

/**
 * Connect temprature to sda/scl pins a4 and a5
 * Connect GPS rx and tx to pins d4 and d5
 * Connect logger rx pin to d2
 **/

Temp temp(0x91>>1);
// RX, TX
GPS gps(4, 5);
GPRS gprs;
SoftwareSerial gsm(2, 3);

const int LED1PIN = 13;
const int LOGLEN = 160;
char logline[LOGLEN];

unsigned long const sensorInterval = 5000; // in milliseconds
unsigned long lastSensor = 0;
unsigned long const gprsInterval = 15000; // in milliseconds
unsigned long lastGPRS = 0;

unsigned long lat = 0, lon = 0, alt = 0;
// for -27 lat
float const latSize = 110803 / 100000; // Meters per 1 lats  0.11
float const lonSize = 99254 / 100000; // Meters per 1 lons  0.09
float const altSize = 0.01;  // Meters per 1 alt
int const latSensitivity = 100 / latSize;
int const lonSensitivity = 100 / lonSize;
int const altSensitivity = 100 / altSize;

void setup()
{
  
  // Start the HW serial port for debug + logging!
  Serial.begin(9600);
  Serial.println("\r\nBalloon logger");
  
  Serial.println(10);
  pinMode(LED1PIN, OUTPUT);
  digitalWrite(LED1PIN, HIGH);
  
  Serial.print("Temp Test : ");
  temp.capture();
  Serial.print(temp.convertCelcius(temp.temperature));
  gsm.begin(9600);
  gps.begin(9600);
  
  Serial.println("Ready!");
  
  digitalWrite(LED1PIN, LOW);
}

void loop()
{
  
  unsigned long currMillis = millis();
  
  if (currMillis - lastSensor > sensorInterval) 
  {
    
    digitalWrite(LED1PIN, HIGH);
    /*for (int i = 0; i < LOGLEN; i++) {
      logline[i] = 0;
    }*/
    
    lastSensor = currMillis;
    
    // Capture GPS for use every loop
    gps.capture();
  
    // Temp Every loop
    temp.capture();
  
    // Log  Every loop stuff that isn't already being logged, e.g. temp
    write_log();
    
    if (currMillis - lastGPRS > gprsInterval) 
    {
      lastGPRS = currMillis;
      // Send via cell  Every 5 mins if reception Can save power by not retrying reception after we lose it until we are back in altitude
        // tcp
        // sms
      send_log();
    }
    
    digitalWrite(LED1PIN, LOW);
  } else {
    // Passthrough nmea strings from gps to serial
    while (gps.available()) {
      Serial.write(gps.read());
    }
  }
}

void addData(char* data)
{
  strcat(logline, data);
  strcat(logline, ",");
}

void addData(int data)
{
  char temp[sizeof(int)];
  ltoa(data, temp, 10);
  addData(temp);
}
void addData(long data)
{
  char temp[sizeof(long)];
  ltoa(data, temp, 10);
  addData(temp);
}
void addData(unsigned long data)
{
  char temp[sizeof(long)];
  ltoa(data, temp, 10);
  addData(temp);
}

void write_log()
{
  // use gps time as it is real time and accurate if we have a fix.
  Serial.print("$TEMP,");
  Serial.print(gps.date);
  Serial.print(",");
  Serial.print(gps.time);
  Serial.print(",");
  // Log time since power as well
  Serial.print(millis());
  Serial.print(",");
  Serial.print(temp.temperature);
  Serial.print(",");
  Serial.println(temp.convertCelcius(temp.temperature));
}

void send_log()
{
  // only send sms if we know where we are
  if (gps.fix_age != TinyGPS::GPS_INVALID_AGE) {

    // Don't send if we haven't moved
    if (abs(gps.lat - lat) > latSensitivity ||
      abs(gps.lon - lon) > lonSensitivity ||
      abs(gps.alt - alt) > altSensitivity) {
      // save values
      lat = gps.lat;
      lon = gps.lon;
      alt = gps.alt;
      
      for (int i = 0; i < LOGLEN; i++) {
        logline[i] = 0;
      }
      addData(gps.time);
      addData("T");
      addData(temp.temperature);
      addData("L");
      addData(gps.lat);
      addData("L");
      addData(gps.lon);
      addData("A");
      addData(gps.alt);
      addData("C");
      addData(gps.course);
      addData("S");
      addData(gps.speed);
      // Write to GPRS
      gprs.sendSMS(&gsm, "0411416892", logline);
      //gprs.sendTCP(&gsm, "blanchg.dnsalias.com", 555, logline);
    }
  }
}

void wait_for_wakeup()
{
  // Sleep here can also sleep gps, temp, gprs
}
