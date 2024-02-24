/*
  The circuit (LM35 square side facing you):
  Sensor <------> Arduino
  ------------------------
  - Left (VCC) <---------> 5v
  - Middle (Vout) <------> Analog Pin
  - Right (GND) <--------> GND
*/

const int sensorPin = A0;
const float offsetValue = 0.5;
const float adcResolution = 1023.0;
const float voltageReference = 5.0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  // Read the analog value from the sensor
  int analogValue = analogRead(sensorPin);

  // Converts the analog Value to volatge
  float voltage = (voltageReference / adcResolution) * analogValue;

  // Converts the voltage to temperature in Celsius
  float temperature = (voltage - offsetValue) * 100;

  Serial.print(temperature);

  // Comment when live testing
  delay(1000);
}
