
// #define GPRS_DEBUG 1

#include "GPRS.h"

#include "WProgram.h"

const char pdp[] = "internet"; //"preconnect";
const char username[] = "";
const char password[] = "";

GPRS::GPRS()
{
  #ifdef GPRS_DEBUG
    Serial.write("Debugging GPRS");
  #endif
}

GPRS::~GPRS()
{
}

inline void consume(Stream* stream)
{
  while(stream->available()){
    #ifdef GPRS_DEBUG
      Serial.write(stream->read());
    #else
      stream->read();
    #endif
  }
}

void GPRS::disableEcho(Stream* stream)
{
  #ifdef GPRS_DEBUG
    Serial.println("Disable echo");
  #endif
  stream->println("ATE0");
  consume(stream);
}
void GPRS::storeConfig(Stream* stream)
{
  stream->println("AT&W");
  consume(stream);
}

bool GPRS::configureSMS(Stream* stream)
{
  #ifdef GPRS_DEBUG
    Serial.println("\nConfigure message");
  #endif
  stream->println("AT+CMGF=1");
  if (!readOK(stream))
  {
    #ifdef GPRS_DEBUG
      Serial.println("No OK");
    #endif
    return false;
  }
  
  return true;
}

bool GPRS::ok(Stream* stream)
{
  stream->println("AT");
  if (!readOK(stream))
  {
    #ifdef GPRS_DEBUG
      Serial.println("No OK");
    #endif
    return false;
  }
  return true;
}

bool GPRS::quality(Stream* stream)
{
  
  int buflen = 100;
  char buffer[buflen];

  stream->println("AT+CSQ");
  readUntil(stream, buffer, buflen, '+');
  readUntil(stream, buffer, buflen, '\n');

  #ifdef GPRS_DEBUG
    Serial.print("B: ");
    Serial.println(buffer);
  #endif
  if (!readOK(stream))
  {
    #ifdef GPRS_DEBUG
      Serial.println("No OK");
    #endif
    return false;
  }
  return true;
}

bool GPRS::reset(Stream* stream)
{
  #ifdef GPRS_DEBUG
    Serial.println("\nReset");
  #endif
  stream->println("ATZ");
  if (!readOK(stream))
  {
    #ifdef GPRS_DEBUG
      Serial.println("No OK");
    #endif
    return false;
  }
  
  return true;
}

bool GPRS::sendSMS(Stream* stream, char* number, char* msg)
{

  /*
  AT+CMGF=1   
  AT+CMGS="15921272576" 
  > HELLO9527<ctrl-z > 
  +CMGS: 36 
  OK
  */
  
  if (!configureSMS(stream)) {
    #ifdef GPRS_DEBUG
      Serial.println("\nCould not configure");
    #endif
    return false;
  }
  
  #ifdef GPRS_DEBUG
    Serial.println("\nSend phone number");
  #endif
  stream->print("AT+CMGS=\"");
  stream->print(number);
  stream->println("\"");
  #ifdef GPRS_DEBUG
    Serial.println("\nWait for prompt");
  #endif
  if (!readPrompt(stream))
  {
    #ifdef GPRS_DEBUG
      Serial.println("No Prompt");
    #endif
    return false;
  }
  
  stream->print(msg);
  stream->print(26, BYTE);
  int buflen = 100;
  char buffer[buflen];
  
  #ifdef GPRS_DEBUG
    Serial.println("\nWait for response");
  #endif
  if (!matchChars(stream, "+CMGS:")) {
    #ifdef GPRS_DEBUG
      Serial.println("No +CMGS:");
    #endif
    return false;
  }
  if (!readUntil(stream, buffer, buflen, '\n'))
  {
    #ifdef GPRS_DEBUG
      Serial.println("No new line");
    #endif
    return false;
  }
  
  #ifdef GPRS_DEBUG
    Serial.println("\nWait for OK");
  #endif
  if (!readOK(stream))
  {
    #ifdef GPRS_DEBUG
      Serial.println("No OK 2");
    #endif
    return false;
  }
  
  return true;
}

// Taken from http://www.sparkfun.com/datasheets/CellularShield/SM5100B%20TCPIP%20App%20Note.pdf
bool GPRS::sendTCP(Stream* stream, char* host, int port, char* data)
{
  int buflen = 100;
  char buffer[buflen]; 


  #ifdef GPRS_DEBUG
    Serial.println("AT+CGATT?");
  #endif
  stream->println("AT+CGATT?");
  
  if (!matchChars(stream, "+CGATT: 1"))
  {
    #ifdef GPRS_DEBUG
      Serial.println("Could not find CGATT : 1");
    #endif
    return false;
  }
  if (!readOK(stream)) {
    #ifdef GPRS_DEBUG
      Serial.println("Not ok");
    #endif
    return false;
  }
  
  /*#ifdef GPRS_DEBUG
    Serial.println("AT+CGDCONT=?");
  #endif
  // It appears that this should only happen once, however there is no real way to test it easily
  stream->println("AT+CGDCONT?");
  if (readOK(stream))
  {
    #ifdef GPRS_DEBUG
      Serial.print("AT+CGDCONT=1,\"IP\",\"");
      Serial.print(pdp);
      Serial.println("\"");
    #endif
    stream->print("AT+CGDCONT=1,\"IP\",\"");
    stream->print(pdp);
    stream->println("\"");
    if (!readOK(stream)) {
      #ifdef GPRS_DEBUG
        Serial.println("Not ok");
      #endif
      return false;
    }
  } 
  else 
  {
    #ifdef GPRS_DEBUG
      Serial.println("Have context already");
    #endif
    consume(stream);
  }
  */
  #ifdef GPRS_DEBUG
    Serial.print("AT+CGDCONT=1,\"IP\",\"");
    Serial.print(pdp);
    Serial.println("\"");
  #endif
  stream->print("AT+CGDCONT=1,\"IP\",\"");
  stream->print(pdp);
  stream->println("\"");
  if (!matchChars(stream, "OK", -1, "+CME ERROR: 4")) {
    #ifdef GPRS_DEBUG
      Serial.println("Not ok or error 4");
    #endif
    return false;
  }
  if (strlen(username) > 0) {
    #ifdef GPRS_DEBUG
      Serial.println("Username/Password");
    #endif
    stream->print("AT+CGPCO=0,\"");
    stream->print(username);
    stream->print("\",\"");
    stream->print(password);
    stream->println("\", 1 ");
    if (!readOK(stream)) {
      #ifdef GPRS_DEBUG
        Serial.println("Not ok");
      #endif
      return false;
    }
  }
  
  #ifdef GPRS_DEBUG
    Serial.println("AT+CGACT=1,1");
  #endif
  stream->println("AT+CGACT=1,1");
  if (!readOK(stream)) {
    #ifdef GPRS_DEBUG
      Serial.println("Not ok");
    #endif
    return false;
  }
  
  #ifdef GPRS_DEBUG
    Serial.print("AT+SDATACONF=1,\"TCP\",\"");
    Serial.print(host);
    Serial.print("\",");
    Serial.println(port);
  #endif
  stream->print("AT+SDATACONF=1,\"TCP\",\"");
  stream->print(host);
  stream->print("\",");
  stream->println(port);
  if (!readOK(stream)) {
    #ifdef GPRS_DEBUG
      Serial.println("Not ok");
    #endif
    return false;
  }
  
  #ifdef GPRS_DEBUG
    Serial.println("AT+SDATASTART=1,1");
  #endif
  stream->println("AT+SDATASTART=1,1");
  if (!readOK(stream)) {
    #ifdef GPRS_DEBUG
      Serial.println("Not ok");
    #endif
    return false;
  }
  bool connected = false;
    #ifdef GPRS_DEBUG
    Serial.println("AT+SDATASTATUS=1");
  #endif
  while (!connected) {
    stream->println("AT+SDATASTATUS=1");
    readUntil(stream, buffer, buflen, '+');
    readUntil(stream, buffer, buflen, '\n');
    // Expecting
    //+SOCKSTATUS:  1,0,0104,0,0,0

    #ifdef GPRS_DEBUG
      Serial.print("B: ");
      Serial.println(buffer);
    #endif

    if (strcmp(buffer, "SOCKSTATUS:  1,1,0102,0,0,0\r") == 0) {
      connected = true;
    }
  }
  #ifdef GPRS_DEBUG
    Serial.print("AT+SDATATSEND=1,");
    Serial.println(strlen(data));
  #endif
  stream->print("AT+SDATATSEND=1,");
  stream->println(strlen(data));
  if (!readOK(stream)) {
    #ifdef GPRS_DEBUG
      Serial.println("Not ok");
    #endif
    return false;
  }
  if(!readTcpPrompt(stream)) {
    #ifdef GPRS_DEBUG
      Serial.println("No prompt");
    #endif
    return false;
  }
  #ifdef GPRS_DEBUG
    Serial.print(data);
  #endif
  stream->print(data);
  stream->print(26, BYTE);
  if (!readOK(stream)) {
    #ifdef GPRS_DEBUG
      Serial.println("Not ok");
    #endif
    return false;
  }
  
  #ifdef GPRS_DEBUG
    Serial.println("AT+SDATASTART=1,0");
  #endif
  stream->println("AT+SDATASTART=1,0");
  if (!readOK(stream)) {
    #ifdef GPRS_DEBUG
      Serial.println("Not ok");
    #endif
    return false;
  }
  return true;
}

bool GPRS::readOK(Stream* stream)
{
  bool result = matchChars(stream, "OK");
  consume(stream);
  return result;
}

bool GPRS::readPrompt(Stream* stream)
{
  bool result = matchChars(stream, "> ", -1, "OK\n\n> ");
  consume(stream);
  return result;
}

bool GPRS::readTcpPrompt(Stream* stream)
{
  bool result = matchChars(stream, "> ", -1);
  consume(stream);
  return result;
}

void GPRS::testMatchChars()
{ 
  
  Serial.println("Type OK or + in 10000ms");
  if (!matchChars(&Serial, "OK", 10000, "+")) {
    Serial.println("Not found");
  }
  
}

bool GPRS::matchChars(Stream* stream, char* match, long timeout, char* alt1)
{
  char* selection = match;
  unsigned long maxWait = millis() + timeout;
  int length = strlen(match);
  int alt1Length = 0;
  if (alt1 != 0) {
   alt1Length = strlen(alt1);
  }
  int index = 0;
  int max = 100;
  int count = 0;
  while(true) {
    if (stream->available()) {
      char c = stream->read();
      #ifdef GPRS_DEBUG
        Serial.write(c);
      #endif
      if (c == '\r' || c == '\n') {
      } else {
        if (c == selection[index])
        {
        } else if (alt1 != 0 && selection != alt1 && index < alt1Length && c == alt1[index]) {
          selection = alt1;
          length = strlen(alt1);
        } else {
          #ifdef GPRS_DEBUG
            Serial.println();
            Serial.print("Could not match: ");
            Serial.print(c, HEX);
            Serial.print(" with ");
            Serial.println(match[index], HEX);
          #endif
          consume(stream);
          return false;
        }

        index ++;
        if (index >= length)
        {
          return true;
        }
      }
    } else if (timeout != -1) {
      unsigned long currMillis = millis();
      if (currMillis > maxWait)
      {
        #ifdef GPRS_DEBUG
          Serial.println("Reached timeout");
        #endif
        return false;
      }
    }
  }
}

bool GPRS::readUntil(Stream* stream, char* buffer, int buflen, char match, long timeout)
{
  unsigned long maxWait = millis() + timeout;
  int index = 0;
  while(true) {
    if (stream->available())
    {
      char c = stream->read();
      #ifdef GPRS_DEBUG
        Serial.write(c);
      #endif
      if (c == match)
      {
        
        buffer[index] = 0;
        return true;
      }
      buffer[index] = c;
      index++;
      if (index >= buflen)
      {
        return false;
      }
    } else if (timeout != -1) {
      unsigned long currMillis = millis();
      if (currMillis > maxWait)
      {
        return false;
      }
    }
  }
}

