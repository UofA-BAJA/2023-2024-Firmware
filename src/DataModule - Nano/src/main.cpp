#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <datamodule.h>

using namespace BAJA_EMBEDDED;

int main() {
  DataModule data_module(DEBUG_LEVEL::COMPLETE);

  data_module.self_identify_type_of_data_module();

  data_module.data_module_initialization_procedure();

  data_module.data_module_operating_procedure();  

  return 0;
}
