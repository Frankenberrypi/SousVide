#include <OneWire.h>

float lowSet = 30; // Low temp where relay turns on
//float highSet = 134.6; // High temp where relay turns off

OneWire  ds(3);  // on pin 3 (a 4.7K resistor is necessary)

long timeN;
double secs;
double mins;
double hrs;

//Relay #8 controlled by thermostat, pin 12
int relayPin8 = 12;

// Red preheat light, pin 10
int preheating = 10;

// Green ready light, pin 11
int running = 11;

// Variable to control LED indicator lights
boolean preheat = true;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin8, OUTPUT);      // sets the digital pin as output  
  digitalWrite(relayPin8, HIGH);        // Prevents relays from starting up engaged
  
  // indicator light pin settings
  pinMode(preheating, OUTPUT);
  pinMode(running, OUTPUT);
  
  // start with the preheating light on, the running light off
  digitalWrite(preheating, HIGH);
  digitalWrite(running, LOW);
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }
  
  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  
  //Time calculation
  timeN = millis();
  secs = timeN / 1000;
  mins = secs / 60;
  hrs = mins / 60;
  
  /*----------
  Serial output, read with python script
  ----------*/
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(hrs,4);
  Serial.print(",");
  Serial.println(fahrenheit);
  //Serial.println(" Fahrenheit");
  
  // Relay control section, limits set in lines 3 and 4
  if (celsius < lowSet) {
    relayOn (relayPin8);
  } else {
    relayOff (relayPin8);
  }
  
  Serial.print(preheat);
  // Indicator light control
  if ((celsius > lowSet) && (preheat = true)){
    preheat = false;
    digitalWrite(preheating, LOW);
    digitalWrite(running, HIGH);
  }
}

int relayOn (int relayNumber) {digitalWrite(relayNumber, LOW);}

int relayOff (int relayNumber) {digitalWrite(relayNumber,HIGH);}
