#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

// Create the bluefruit object

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}
/* ----GUIDS---- */
//IOT Identifier
#define IOTIDENTSERVICE     "0D-BB-9A-2D-39-DE-45-90-8E-AC-BF-DE-F6-50-41-6F"
#define IOTIDENTNAMECHAR    "3C-66-21-11-38-F8-4A-8E-BD-D6-1E-0A-5F-40-32-77"

//LED Power
#define LEDPOWERSERVICE     "A3-AA-7E-F1-FC-73-42-4A-83-A8-5C-99-A9-05-A4-D3"
#define LEDPOWERSTATUSCHAR  "B4-9B-1D-22-2C-A7-44-8B-B9-8A-11-85-C5-32-85-53"

/* ----Advertisements---- */
//Bluetooth Flags
#define FLAGSADV            "02-01-06"
//Device Information Service
#define SERVICES16BITADV    "03-02-0A-18"
//IOT Identifier Service 
#define SERVICES128BITADV   "11-06-6F-41-50-F6-DE-BF-AC-8E-90-45-DE-39-2D-9A-BB-0D"  

/* ----Custom Service ID's---- */
int32_t iotIdentServiceId;
int32_t iotIdentNameCharId;
int32_t ledPowerServiceId;
int32_t ledPowerStatusCharId;

void setup(void)
{
  while (!Serial); // required for Flora & Micro
  delay(500);

  boolean success;

  Serial.begin(115200);
  Serial.println(F("Bluetfruit LED Clock Initialization"));
  Serial.println(F("---------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  /* Perform a factory reset */
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  // ble.setInterCharWriteDelay(5); // 5 ms

  /* Change the device name */
  Serial.println(F("Setting device name to 'Bluefruit LED Clock': "));

  if (! ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Bluefruit LED Clock")) ) {
    error(F("Could not set device name?"));
  }

  /* Add the IOT Identifier Service */
  Serial.println(F("Adding the IOT Identifier Service (UUID = " IOTIDENTSERVICE "): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDSERVICE=UUID128=" IOTIDENTSERVICE), &iotIdentServiceId);
  if (! success) {
    error(F("Could not add IOT Identifier service"));
  }

  /* Add the IOT Identifier Name Characteristic */
  Serial.println(F("Adding the LED Power Status characteristic (UUID = " IOTIDENTNAMECHAR "): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID128=" IOTIDENTNAMECHAR 
    ", PROPERTIES=0x02, MIN_LEN=9, MAX_LEN=9, VALUE=WELLS_IOT"), &iotIdentNameCharId);
  if (! success) {
    error(F("Could not add IOT Identifier Name characteristic"));
  }
  
  /* Add the LED Power Service */
  Serial.println(F("Adding the LED Power Service (UUID = " LEDPOWERSERVICE "): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDSERVICE=UUID128=" LEDPOWERSERVICE), &ledPowerServiceId);
  if (! success) {
    error(F("Could not add LED Power service"));
  }

  /* Add the LED Power Status Characteristic */
  Serial.println(F("Adding the LED Power Status characteristic (UUID = " LEDPOWERSTATUSCHAR "): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID128=" LEDPOWERSTATUSCHAR 
    ", PROPERTIES=0x1A, MIN_LEN=2, MAX_LEN=3, VALUE=OFF"), &ledPowerStatusCharId);
  if (! success) {
    error(F("Could not add LED Power Status characteristic"));
  }
  
  /* Add IOT Identifier Service to the advertising packet */
  Serial.print(F("Creating advertising payload: "));
  ble.sendCommandCheckOK( F("AT+GAPSETADVDATA=" FLAGSADV "-" SERVICES16BITADV "-" SERVICES128BITADV) );

  /* Reset the device for the new service setting changes to take effect */
  Serial.print(F("Performing a SW reset (service changes require a reset): "));
  ble.reset();

  Serial.println();
}

void loop(void)
{
  /*
  int heart_rate = random(50, 100);

  Serial.print(F("Updating HRM value to "));
  Serial.print(heart_rate);
  Serial.println(F(" BPM"));

  // Command is sent when \n (\r) or println is called 
  // AT+GATTCHAR=CharacteristicID,value 
  ble.print( F("AT+GATTCHAR=") );
  ble.print( hrmMeasureCharId );
  ble.print( F(",00-") );
  ble.println(heart_rate, HEX);

  // Check if command executed OK 
  if ( !ble.waitForOK() )
  {
    Serial.println(F("Failed to get response!"));
  }
  */
  /* Delay before next measurement update */
  delay(1000);
}
