#include <avr/io.h>
#include <avr/interrupt.h>

#include "macros.h"

#include "imu.h"
#include "i2c.h"

#define SLA 0x68  // MPU_6050 address with PIN AD0 grounded

//all the register addresses for the MPU6050
#define PWR_MGMT  0x6B
#define WAKEUP 0x00
#define SL_MEMA_XAX_HIGH  0x3B
#define SL_MEMA_XAX_LOW   0x3C
#define SL_MEMA_YAX_HIGH  0x3D
#define SL_MEMA_YAX_LOW   0x3E
#define SL_MEMA_ZAX_HIGH  0x3F
#define SL_MEMA_ZAX_LOW   0x40
#define SL_TEMP_HIGH      0x41
#define SL_TEMP_LOW       0x42

void IMU_DataModule::data_module_setup_procedure() {}
void IMU_DataModule::data_module_logging_procedure(){}

// void IMU_DataModule::data_module_setup_procedure() 
// {
//   sei(); 

//   initI2C(); 
  
//   /* Initialise the sensor */
//   if(!bno.begin())
//   {
//     /* There was a problem detecting the BNO055 ... check your connections */
//     Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
//     while(1);
//   }
//   _delay_ms(1000);
//   bno.setExtCrystalUse(true);
// }

// void IMU_DataModule::data_module_operating_procedure(){
//   StartSDReading();

//   while(true){
//     // Incoming command from raspberry pi!
//     PollCommand();

//     if(logging){
//       /* Get a new sensor event */ 
//       sensors_event_t event; 
//       bno.getEvent(&event);
//       /* Display the floating point data */
//       String dataString = "";
//       dataString += millis();
//       dataString += "ms  Y:";
//       dataString += event.orientation.y;
//       // Serial.print("X: ");
//       // Serial.print(event.orientation.x, 4);
//       // Serial.print("\tY: ");
//       // Serial.print(event.orientation.y, 4);
//       // Serial.print("\tZ: ");
//       // Serial.print(event.orientation.z, 4);
//       // Serial.println("");
//       WriteToSD(dataString);
//     }
    
//     delay(20);
//   }
// }