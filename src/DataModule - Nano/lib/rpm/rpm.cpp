#include <rpm.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

// ! I feel like you shouldn't need to have this dependency in every single data module...?
#include <SD.h>
#include <SPI.h>

#include "config.h"
#include "datamodule.h"

#define RPM_SENSING_DURATION_PERIOD_MS 20 // time in ms for sensor to collect
#define RPM_NUM_OF_PERIODS_TO_AVG 20      // number of periods to average for rpm calculation
#define NUM_MAGNETS 30                    // number of magnets on the wheel
#define WHEEL_CIRCUMFERENCE 70.5          // circumference of the wheel in inches

#define WHEEL_RADIUS .284998              // in meters
#define ANGLE_BETWEEN_MAGNETS_RAD .4188   // in radians

int rpm_left_counts[RPM_NUM_OF_PERIODS_TO_AVG] = {0};
int rpm_rear_counts[RPM_NUM_OF_PERIODS_TO_AVG] = {0};
int rpm_right_counts[RPM_NUM_OF_PERIODS_TO_AVG] = {0};

volatile int RPM_DataModule::left_rpm_counter = 0;  // Define and initialize the static member
volatile int RPM_DataModule::right_rpm_counter = 0; // Define and initialize the static member
volatile int RPM_DataModule::rear_rpm_counter = 0;  // Define and initialize the static member
volatile float RPM_DataModule::speed = 0;           // volatile float RPM_DataModule::speed = 0; // Define and initialize the static member

float deltaTimes[40];
float prevTime = 0.0; // Prev time in microseconds
int currArrayIndex = 0;

float time = 0.0; // Time passed in microseconds


void RPM_DataModule::data_module_initialization_procedure()
{
    sei(); // Enable interrupts
    initialize_left_rpm_sensor();
    initialize_right_rpm_sensor();

    InitializeSDReading(10, "rpmdata.txt");


#if DEBUG_LEVEL == DEV
    Serial.println("initialized RPM data module");
#endif
}

void RPM_DataModule::data_module_operating_procedure()
{
    StartSDReading();
    bool logging = false;
    while (1)
    {
        /*operating procedure
        1. wait RPM_SENSING_DURATION_PERIOD_MS to accumlate ticks
        2. shift data array left to make space for new array
        3. store new value in data array
        4. avg data points in array
        5. report rpm
        - print to serial
        - write to sd card
        */

           // Incoming command from raspberry pi!
        //    void pollCommandFromPI()
        if(Serial.available() > 0){
        String command = Serial.readString();

        if(command == "Begin Logging"){
            logging = true;
        }
        else if(command == "End Logging"){
            logging = false;
            CloseSD();
        }
        else if(command == "Retrieve Logs"){
            if(logging){
            Serial.println("You are still logging. stop logging first");
            }
            else{
            SendFile();
            }
        }
        }

        _delay_ms(RPM_SENSING_DURATION_PERIOD_MS);

        String dataString = "";

        for(int i = 0; i < currArrayIndex; i++){
            float deltaTime = deltaTimes[i];
            time += deltaTime;

            deltaTime /= 1000000.0; // Convert from microseconds to seconds.

            float calculatedSpeed = (ANGLE_BETWEEN_MAGNETS_RAD / deltaTime) * WHEEL_RADIUS;
            calculatedSpeed *= 2.23694; // convert to mph.

            float timeInMilis = time / 1000.0;

            Serial.print(">speed: ");
            Serial.println(calculatedSpeed);

            if(logging){
                dataString += time;
                dataString += " ";
                dataString += calculatedSpeed;
                dataString += "\n";

                WriteToSD(dataString);
            }

        }
        currArrayIndex = 0;



    #if DEBUG_LEVEL == DEV
            // Serial.println(speed);
            // Serial.print(">rear_rpm: ");
            // Serial.println(rear_rpm);
            // Serial.print(">Speed: ");
            // Serial.println(speed);
            // Serial.print(">left_rpm: ");
            // Serial.println(left_rpm_counter);
            // Serial.print(">right_rpm: ");
            // Serial.println(right_rpm_counter);
            // Serial.print(">left_rpm: ");
            // Serial.println(left_rpm_counter);
            // Serial.print(">right_rpm: ");
            // Serial.println(right_rpm_counter);
    #endif
    }

}

void RPM_DataModule::initialize_left_rpm_sensor()
{

    // setting up rear left rpm sensor
    DDRD &= ~(1 << DDD2);   // set direction for input
    PORTD |= (1 << PORTD2); // enable the pullup resistor for stable input

    EICRA |= (1 << ISC01) | (1 << ISC00); // set INT0 to trigger on rising edge
    EIMSK |= (1 << INT0);                 // enable external interrupt for INT0
}

void RPM_DataModule::initialize_right_rpm_sensor()
{

    // setting up rear right rpm sensor
    DDRD &= ~(1 << DDD3);   // set direction for input
    PORTD |= (1 << PORTD3); // enable the pullup resistor for stable input

    EICRA |= (1 << ISC11) | (1 << ISC10); // set INT1 to trigger on rising edge
    EIMSK |= (1 << INT1);                 // enable external interrupt for INT1
}

void RPM_DataModule::shift_data_array_left(int data_array[], int array_size)
{
    for (int i = 0; i < array_size - 1; i++)
    {
        data_array[i] = data_array[i + 1];
    }
}

float RPM_DataModule::get_average_of_data_array(int data_array[], int array_size)
{
    float sum = 0.0;
    for (int i = 0; i < array_size; i++)
    {
        sum += data_array[i];
    }
    return sum / (float)array_size;
}
float RPM_DataModule::calculate_rpm(float sensor_trigger_count)
{

    float rpm;
    float rotations;

    rotations = sensor_trigger_count / NUM_MAGNETS;

    rpm = (rotations / ((RPM_SENSING_DURATION_PERIOD_MS) / 1000.0)) * 60;

    return rpm;
}

float RPM_DataModule::calculate_speed(float rpm)
{
    float speed;
    speed = ((rpm * WHEEL_CIRCUMFERENCE) / 63360) * 60;
    return speed;
}

ISR(INT0_vect)
{
    float currTime = micros();
    float deltaTime = currTime - prevTime;
    deltaTimes[currArrayIndex] = deltaTime;
    currArrayIndex++;
    prevTime = currTime;

    // RPM_DataModule::left_rpm_counter++;
}

ISR(INT1_vect)
{
    // RPM_DataModule::right_rpm_counter++;
}