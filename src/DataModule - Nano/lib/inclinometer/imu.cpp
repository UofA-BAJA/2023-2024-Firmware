#include <Arduino.h>
#include <imu.h>
#include <config.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


// I'm not sure why these two includes have to be here. I would think that because it inherits from
// The datamodule, it should be fine. However, it doesn't seem to work that way.
#include <SD.h>
#include <SPI.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

void IMU_DataModule::data_module_initialization_procedure() 
{
    pinMode(5, INPUT_PULLUP);

    #if DEBUG_LEVEL == DEV
        Serial.println("Initialized IMU data module");
    #endif

  // SD Reading setup
  InitializeSDReading(10, "RotData.txt");
  
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

  bool logging = false;

  while(true){

    // Incoming command from raspberry pi!
    if(Serial.available() > 0){
      String command = Serial.readString();

      Serial.println(Serial.available());
      if(command == "Begin Logging"){
        logging = true;
      }
      else if(command == "End Logging"){
        logging = false;
        CloseSD();
      }
      else if(command == "Retrieve Logs"){

      }
    }

    if(logging){
      Serial.println("Logging now");
      /* Get a new sensor event */ 
      sensors_event_t event; 
      bno.getEvent(&event);
      /* Display the floating point data */
      String dataString = "";
      dataString += millis();
      dataString += "ms  Y:";
      dataString += event.orientation.y;
      // Serial.print("X: ");
      // Serial.print(event.orientation.x, 4);
      // Serial.print("\tY: ");
      // Serial.print(event.orientation.y, 4);
      // Serial.print("\tZ: ");
      // Serial.print(event.orientation.z, 4);
      // Serial.println("");
      WriteToSD(dataString);
    }
    
    delay(20);
  }
}