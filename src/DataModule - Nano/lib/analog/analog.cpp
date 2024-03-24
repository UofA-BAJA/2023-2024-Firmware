#include "analog.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#include "Arduino.h"

#include "macros.h"
#include "enums.h"

const int analogPin = A5; // Assigns analog pin A0 to analogPin

void BRK_DataModule::set_data_module_type()
{
    strcpy(data_module_type, MODULE_TYPES_BRK);
}

void BRK_DataModule::data_module_setup_procedure() {
    // Set up ADMUX register
    ADMUX = (1 << REFS0); // AVcc as reference and select ADC5
    ADMUX &= ~(1 << REFS1);  // Make sure ADPS0 is set to 0


    // Set up ADCSRA register for prescaler of 64
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1);
    ADCSRA &= ~(1 << ADPS0);  // Make sure ADPS0 is set to 0

    // Clear the MUX bits (MUX3:0) before setting them to the desired channel (ADC5)
    ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
    // Set the MUX bits to select ADC5 (0101)
    ADMUX |= (1 << MUX2) | (1 << MUX0);

        ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescale for 16Mhz
    strcpy(dataHeaderArray[0], DATA_TYPES_BRAKE_PRESSURE_REAR);
    strcpy(dataHeaderArray[1], DATA_TYPES_BRAKE_PRESSURE_FRONT);

    DEBUG_PRINTLN("BRAKE Data Module Setup Complete");

    ADCSRA |= (1 << ADEN); // Enable the ADC
}
void BRK_DataModule::data_module_logging_procedure(){
    ADCSRA |= (1 << ADSC); // Start a conversion
    while (ADCSRA & (1 << ADSC)); // Wait for the conversion to finish
    uint16_t adc_result = ADC; // Read the ADC value

    // Convert the integer ADC result to a float voltage
    float voltage = (float)adc_result * (5.0 / 1024.0); // Convert adc_result to voltage
    dataToRecord[0] = voltage;

    DEBUG_PRINT(">:");
    DEBUG_PRINTLN(voltage);
    recordDataToSDCard();
}

