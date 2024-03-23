#include "analog.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#include "Arduino.h"

#include "macros.h"
#include "enums.h"



void BRK_DataModule::set_data_module_type()
{
    strcpy(data_module_type, MODULE_TYPES_BRK);
}

void BRK_DataModule::data_module_setup_procedure() {
    strcpy(dataHeaderArray[0], DATA_TYPES_BRAKE_PRESSURE_REAR);
    strcpy(dataHeaderArray[1], DATA_TYPES_BRAKE_PRESSURE_FRONT);

}
void BRK_DataModule::data_module_logging_procedure(){
    int adcValue = analogRead(analogPin); // Read the ADC value from the specified pin

    dataToRecord[0] = adcValue;

    recordDataToSDCard();
}

