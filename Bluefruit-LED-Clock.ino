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

//LED Power
#define LEDPOWERSERVICE     "A3-AA-7E-F1-FC-73-42-4A-83-A8-5C-99-A9-05-A4-D3"
#define LEDPOWERSTATUSCHAR  "B4-9B-1D-22-2C-A7-44-8B-B9-8A-11-85-C5-32-85-53"

/* The service information */

int32_t ledPowerServiceId;
int32_t ledPowerStatusCharId;
/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module
*/
/**************************************************************************/
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

  /* Perform a factory reset to make sure everything is in a known state */
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  // this line is particularly required for Flora, but is a good idea
  // anyways for the super long lines ahead!
  // ble.setInterCharWriteDelay(5); // 5 ms

  /* Change the device name to make it easier to find */
  Serial.println(F("Setting device name to 'Bluefruit LED Clock': "));

  if (! ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Bluefruit LED Clock")) ) {
    error(F("Could not set device name?"));
  }

  /* Add the LED Power Service definition */
  /* Service ID should be 1 */
  Serial.println(F("Adding the LED Power Service (UUID = " LEDPOWERSERVICE "): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDSERVICE=UUID128=" LEDPOWERSERVICE), &ledPowerServiceId);
  if (! success) {
    error(F("Could not add HRM service"));
  }

  /* Add the Heart Rate Measurement characteristic */
  /* Chars ID for Measurement should be 1 */
  Serial.println(F("Adding the LED Power Status characteristic (UUID = " LEDPOWERSTATUSCHAR "): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID128=" LEDPOWERSTATUSCHAR 
    ", PROPERTIES=0x1A, MIN_LEN=1, MAX_LEN=3, VALUE=OFF"), &ledPowerStatusCharId);
  if (! success) {
    error(F("Could not add HRM characteristic"));
  }
  /*
  // Add the Heart Rate Service to the advertising data (needed for Nordic apps to detect the service) 
  Serial.print(F("Adding Heart Rate Service UUID to the advertising payload: "));
  ble.sendCommandCheckOK( F("AT+GAPSETADVDATA=" + LEDPOWERSERVICE) );
  */
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
