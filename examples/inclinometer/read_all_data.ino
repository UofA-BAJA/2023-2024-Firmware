#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
a
   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3-5V DC
   Connect GROUND to common ground

   History
   =======
   2015/MAR/03  - First release (KTOWN)
*/

/* Set the delay between fresh samples */
uint16_t BNO055_SAMPLERATE_DELAY_MS = 500;

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup(void)
{
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  Serial.begin(115200);

  while (!Serial) delay(10);  // wait for serial port to open!

  Serial.println("Orientation Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(1000);
}

void loop(void)
{
  //could add VECTOR_ACCELEROMETER, VECTOR_MAGNETOMETER,VECTOR_GRAVITY...
  sensors_event_t orientationData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

  printEvent(&orientationData);
  
  int8_t boardTemp = bno.getTemp();
  Serial.println();
  Serial.print(F("temperature: "));
  Serial.println(boardTemp);

//  uint8_t system, gyro, accel, mag = 0;
//  bno.getCalibration(&system, &gyro, &accel, &mag);
//  Serial.println();
//  Serial.print("Calibration: Sys=");
//  Serial.print(system);
//  Serial.print(" Gyro=");
//  Serial.print(gyro);
//  Serial.print(" Accel=");
//  Serial.print(accel);
//  Serial.print(" Mag=");
//  Serial.println(mag);

  Serial.println("--");
  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void printEvent(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem

  if (event->type == SENSOR_TYPE_ORIENTATION) {
    Serial.print("Orient:");
    y = event->orientation.y;
    z = event->orientation.z;
  }

  else {
    Serial.print("Unk:");
  }

  x = x - (ceil((x + 180)/360)-1)*360;           // (-180;180]:  y = fmodf((y - 180.0), 180.0);
  y = y - (ceil((y + 180)/360)-1)*360;           // (-180;180]:  y = fmodf((y - 180.0), 180.0);
  z = z - (ceil((z + 180)/360)-1)*360;           // (-180;180]:  y = fmodf((y - 180.0), 180.0);

  double yOutput = (abs(y) > 45) ? 200 : 0;
  analogWrite(9, yOutput);
  double zOutput = (abs(z) > 45) ? 200 : 0;
  analogWrite(10, zOutput);

//  analogWrite(11, abs(z));


  Serial.print("\tx= ");
  Serial.print(x);
  Serial.print(" |\ty= ");
  Serial.print(y);
  Serial.print(" |\tz= ");
  Serial.println(z);
}
