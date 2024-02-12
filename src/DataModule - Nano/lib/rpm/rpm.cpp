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

int rpm_left_counts[RPM_NUM_OF_PERIODS_TO_AVG] = {0};
int rpm_rear_counts[RPM_NUM_OF_PERIODS_TO_AVG] = {0};
int rpm_right_counts[RPM_NUM_OF_PERIODS_TO_AVG] = {0};

volatile int RPM_DataModule::left_rpm_counter = 0;  // Define and initialize the static member
volatile int RPM_DataModule::right_rpm_counter = 0; // Define and initialize the static member
volatile int RPM_DataModule::rear_rpm_counter = 0;  // Define and initialize the static member
volatile float RPM_DataModule::speed = 0;           // volatile float RPM_DataModule::speed = 0; // Define and initialize the static member

void RPM_DataModule::data_module_initialization_procedure()
{
    sei(); // Enable interrupts
    initialize_left_rpm_sensor();
    initialize_right_rpm_sensor();

    InitializeSDReading(10, "p.txt");


#if DEBUG_LEVEL == DEV
    Serial.println("initialized RPM data module");
#endif
}

void RPM_DataModule::data_module_operating_procedure()
{
    StartSDReading();
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
        PollCommand();

        _delay_ms(RPM_SENSING_DURATION_PERIOD_MS);
        rear_rpm_counter = left_rpm_counter + right_rpm_counter;

        shift_data_array_left(rpm_rear_counts, RPM_NUM_OF_PERIODS_TO_AVG);
        rpm_rear_counts[RPM_NUM_OF_PERIODS_TO_AVG - 1] = rear_rpm_counter;
        rear_rpm_counter = 0;

        left_rpm_counter = 0;
        right_rpm_counter = 0;

        avg_rear_rpm_count = get_average_of_data_array(rpm_rear_counts, RPM_NUM_OF_PERIODS_TO_AVG);

        rear_rpm = calculate_rpm(avg_rear_rpm_count);

        // shift_data_array_left(rpm_left_counts, RPM_NUM_OF_PERIODS_TO_AVG);
        // rpm_left_counts[RPM_NUM_OF_PERIODS_TO_AVG - 1] = left_rpm_counter;
        // left_rpm_counter = 0;

        // avg_left_rpm_count = get_average_of_data_array(rpm_left_counts, RPM_NUM_OF_PERIODS_TO_AVG);

        // left_rpm = calculate_rpm(avg_left_rpm_count);

        // shift_data_array_left(rpm_right_counts, RPM_NUM_OF_PERIODS_TO_AVG);
        // rpm_right_counts[RPM_NUM_OF_PERIODS_TO_AVG - 1] = right_rpm_counter;
        // right_rpm_counter = 0;

        // avg_right_rpm_count = get_average_of_data_array(rpm_right_counts, RPM_NUM_OF_PERIODS_TO_AVG);

        // right_rpm = calculate_rpm(avg_right_rpm_count);

        speed = calculate_speed(rear_rpm);
        if(logging){
            String dataString = "";
            dataString += millis();
            dataString += " ms | ";
            dataString += speed;
            dataString += " mph";
            WriteToSD(dataString);
        }
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
    RPM_DataModule::left_rpm_counter++;
}

ISR(INT1_vect)
{
    RPM_DataModule::right_rpm_counter++;
}