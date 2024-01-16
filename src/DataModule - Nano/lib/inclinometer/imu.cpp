#include <Arduino.h>
#include <imu.h>
#include <config.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

  
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void IMU_DataModule::data_module_initialization_procedure() 
{
    #if DEBUG_LEVEL == DEV
        Serial.println("Initialized IMU data module");
    #endif

  // SD Reading setup
  InitializeSDReading(10, "rotations.txt");
  
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

void IMU_DataModule::data_module_operating_procedure(){
  StartSDReading();
  while(true){
    /* Get a new sensor event */ 
    sensors_event_t event; 
    bno.getEvent(&event);
    /* Display the floating point data */
    String dataString = millis() + "ms  X:" + (String)event.orientation.y;
    // Serial.print("X: ");
    // Serial.print(event.orientation.x, 4);
    // Serial.print("\tY: ");
    // Serial.print(event.orientation.y, 4);
    // Serial.print("\tZ: ");
    // Serial.print(event.orientation.z, 4);
    // Serial.println("");
    
    delay(20);
  }
}