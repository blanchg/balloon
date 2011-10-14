
#include <SoftwareSerial.h>
#include <GPRS.h>

SoftwareSerial gsm(2,3);                                                                                                                                                                   
GPRS gprs;

const int loglen = 1000;
char logline[loglen];

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup");
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  gsm.begin(9600);
  delay(1000);
  
  gprs.disableEcho(&gsm);
  gprs.storeConfig(&gsm);
  bool result = false;
  digitalWrite(13, LOW);
  
}

void loop()
{
    if (Serial.available()) {
      digitalWrite(13, HIGH);
      unsigned long time = millis();
      char c = Serial.read();
      if (c == 'r')
      {
       gprs.reset(&gsm);
      }
      else if (c == 10) // Enter key 
      {
        sprintf(logline, "Took %lu seconds to send this", time / 1000);
        bool result = gprs.sendSMS(&gsm, "0411416892", logline);
        Serial.print(logline);
        Serial.print(": ");
        Serial.println(result);
        digitalWrite(13, LOW);
      }
    }
    while(gsm.available()) {
      Serial.write(gsm.read());
    }
}
