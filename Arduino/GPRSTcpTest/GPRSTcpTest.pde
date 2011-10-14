
#include <SoftwareSerial.h>
#include <GPRS.h>

GPRS gprs;
SoftwareSerial gsm(2, 3);

char hostname[100] = "115.131.7.19";
int hostnamelen = strlen(hostname);
char mobile[] = "0411416892";

void setup()
{
  Serial.begin(9600);
  Serial.println("Start TCP Test");
  
  gsm.begin(9600);
  
  //gprs.setPDP("internet"); // could also pass username/password
}

void loop()
{
  if (Serial.available()) {
    char c = Serial.read();
    Serial.println(c, HEX);
    if (c == 27) { // Esc
      for (int i = 0; i < 100; i++) {
        hostname[i] = 0;
      }
      hostnamelen = 0;
    } else if ((c >= '0' && c <= '9') || c == '.') {
      hostname[hostnamelen] = c;
      hostnamelen++;
    } else if (c == 0x7F) {// Backspace
      hostnamelen--;
      hostname[hostnamelen] = 0;
    } else if ((c == 't' || c == 0xD) && hostnamelen > 0) { // Enter
      Serial.print("Sending via TCP to ");
      Serial.println(hostname);
      char logline[100];
      sprintf(logline, "Hi there %s: %lums\n", hostname, millis());
      if (gprs.sendTCP(&gsm, hostname, 555, logline)) {
        Serial.println("Sent via tcp");
      } else {
        Serial.println("Failed");
      }
    } else if (c == 's') {
      Serial.print("Sending via SMS to ");
      Serial.println(mobile);
      char logline[100];
      sprintf(logline, "Hi there %s: %lums", mobile, millis());
      if (gprs.sendSMS(&gsm, mobile, logline)) {
        Serial.println("Sent via sms");
      } else {
        Serial.println("Failed");
      }
    } else if (c == 'o') {
      gprs.ok(&gsm);
    } else if (c == 'q') {
      gprs.quality(&gsm);
    } else if (c == 'r') {
      gprs.reset(&gsm);
    }
    Serial.print("Hostname: ");
    Serial.println(hostname);
  }
  while (gsm.available()) {
    Serial.write(gsm.read());
  }
}
  
