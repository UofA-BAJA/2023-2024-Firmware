#include <rpm.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <HardwareSerial.h>

// ! I feel like you shouldn't need to have this dependency in every single data module...?
#include <SD.h>
#include <SPI.h>

#include "datamodule.h"
#include "enums.h"
#include "macros.h"
#include "timer.h"

// #define RPM_SENSING_DURATION_PERIOD_MS 20 // time in ms for sensor to collect
// #define RPM_NUM_OF_PERIODS_TO_AVG 20      // number of periods to average for rpm calculation
#define NUM_MAGNETS 30                    // number of magnets on the wheel
// #define WHEEL_CIRCUMFERENCE 70.5          // circumference of the wheel in inches
#define WHEEL_RADIUS .284998              // in meters
#define ANGLE_BETWEEN_MAGNETS_RAD .4188   // in radians

volatile float RPM_DataModule::leftSpeed = 0;
volatile float RPM_DataModule::rightSpeed = 0;           // volatile float RPM_DataModule::speed = 0; // Define and initialize the static member

volatile int left_currArrayIndex = 0;
volatile int right_currArrayIndex = 0;
volatile unsigned long left_prevTime = 0; // Prev time in microseconds
volatile unsigned long right_prevTime = 0; // Prev time in microseconds 
volatile unsigned long left_deltatimes[20]; // Time passed in microseconds
volatile unsigned long right_deltatimes[20]; // Time passed in microseconds

volatile unsigned long time = 0; // Time passed in microseconds

void RPM_DataModule::set_data_module_type()
{
    strcpy(data_module_type, MODULE_TYPES_RPM_REAR);
}

void RPM_DataModule::data_module_setup_procedure()
{
    sei(); // Enable interrupts
    initialize_left_rpm_sensor();
    initialize_right_rpm_sensor();
    initTimer2();

    DEBUG_PRINTLN("RPM Data Module Setup Complete");

    strcpy(dataHeaderArray[0], DATATYPES_RPM_FRONT_LEFT);
    strcpy(dataHeaderArray[1], DATATYPES_RPM_FRONT_RIGHT);

    startTimer2();
}

void RPM_DataModule::data_module_logging_procedure() {
    
    for(int i = 0; i < left_currArrayIndex; i++){
        float leftDeltaTimeSeconds = left_deltatimes[i] / 1000000.0; // Convert from milliseconds to seconds.
    
        leftSpeed = (ANGLE_BETWEEN_MAGNETS_RAD / leftDeltaTimeSeconds) * WHEEL_RADIUS; //angular velocity * radius = speed m/s

        leftSpeed *= 2.23694; // convert to mph.

        dataToRecord[0] = leftSpeed;
        dataToRecord[1] = -2.0;

        // DEBUG_PRINT(">left: ");
        // DEBUG_PRINTLN(leftSpeed);
        recordDataToSDCard();
    }


    for(int i = 0; i < right_currArrayIndex; i++){
        float rightDeltaTimeSeconds = right_deltatimes[i] / 1000000.0; // Convert from milliseconds to seconds.
    
        rightSpeed = (ANGLE_BETWEEN_MAGNETS_RAD / rightDeltaTimeSeconds) * WHEEL_RADIUS; //angular velocity * radius = speed m/s

        rightSpeed *= 2.23694; // convert to mph.

        dataToRecord[0] = -2.0;
        dataToRecord[1] = rightSpeed;

        recordDataToSDCard();
    }

    left_currArrayIndex = 0;
    right_currArrayIndex = 0;
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

// void RPM_DataModule::shift_data_array_left(int data_array[], int array_size)
// {
//     for (int i = 0; i < array_size - 1; i++)
//     {
//         data_array[i] = data_array[i + 1];
//     }
// }

// float RPM_DataModule::get_average_of_data_array(int data_array[], int array_size)
// {
//     float sum = 0.0;
//     for (int i = 0; i < array_size; i++)
//     {
//         sum += data_array[i];
//     }
//     return sum / (float)array_size;
// }
// float RPM_DataModule::calculate_rpm(float sensor_trigger_count)
// {

//     float rpm;
//     float rotations;

//     rotations = sensor_trigger_count / NUM_MAGNETS;

//     rpm = (rotations / ((RPM_SENSING_DURATION_PERIOD_MS) / 1000.0)) * 60;

//     return rpm;
// }

// float RPM_DataModule::calculate_speed(float rpm)
// {
//     float speed;
//     speed = ((rpm * WHEEL_CIRCUMFERENCE) / 63360) * 60;
//     return speed;
// }

ISR(INT0_vect) //int0 is the left side
{
    unsigned long left_curr_time = readMicrosecondsElapsedFromTimer2();
    left_deltatimes[left_currArrayIndex] =  left_curr_time - left_prevTime;
    left_currArrayIndex++;
    left_prevTime = left_curr_time;
    // RPM_DataModule::left_rpm_counter++;
}

ISR(INT1_vect)
{
    unsigned long right_curr_time = readMicrosecondsElapsedFromTimer2();
    right_deltatimes[right_currArrayIndex] = right_curr_time - right_prevTime;
    right_currArrayIndex++;
    right_prevTime = right_curr_time;
}
   