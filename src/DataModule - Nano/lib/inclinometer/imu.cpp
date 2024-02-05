#include <SD.h>
#include <SPI.h>
#include <HardwareSerial.h>

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


void IMU_DataModule::data_module_setup_procedure() 
{
  sei(); 

  initI2C(); 
  
  /* Initialise the sensor */
  StartI2C_Trans(SLA);
  //status = TWSR & 0xF8;
  write(PWR_MGMT);// address on SLA for Power Management
  write(WAKEUP); // send data to Wake up from sleep mode
  StopI2C_Trans();

  #if DEBUG_LEVEL == DEV
    Serial.println("Initialized IMU data module");
  #endif
}

void IMU_DataModule::data_module_logging_procedure(){
  
    
}