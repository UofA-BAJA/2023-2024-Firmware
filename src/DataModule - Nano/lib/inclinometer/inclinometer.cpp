// #include <Arduino.h>
#include <inclinometer.h>
#include <config.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void Inclinometer_DataModule::data_module_initialization_procedure() 
{
    #if DEBUG_LEVEL == DEV
        Serial.println("Initialized Inclinometer data module");
    #endif

  // Serial.println("Orientation Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */

  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  _delay_ms(1000);
    
  bno.setExtCrystalUse(true);
}

void Inclinometer_DataModule::data_module_operating_procedure(){

  while(true){
    _delay_ms(100);
  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  
  /* Display the floating point data */
  Serial.print("X: ");
  Serial.print(event.orientation.x, 4);
  Serial.print("\tY: ");
  Serial.print(event.orientation.y, 4);
  Serial.print("\tZ: ");
  Serial.print(event.orientation.z, 4);
  Serial.println("");
  
  delay(100);
  }
}

// void loop(void) 
// {
//   /* Get a new sensor event */ 
//   sensors_event_t event; 
//   bno.getEvent(&event);
  
//   /* Display the floating point data */
//   Serial.print("X: ");
//   Serial.print(event.orientation.x, 4);
//   Serial.print("\tY: ");
//   Serial.print(event.orientation.y, 4);
//   Serial.print("\tZ: ");
//   Serial.print(event.orientation.z, 4);
//   Serial.println("");
  
//   delay(100);
// }