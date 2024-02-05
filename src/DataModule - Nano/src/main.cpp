#include <HardwareSerial.h>

#include "datamodule.h"


#define NONE 0 //no serial output, note that this will siginicantly reduce the size of the binary (FLASH)
#define DEV 1 //all serial output, for development

#define DEBUG_LEVEL DEV

using namespace BAJA_EMBEDDED;

int main() {
  #if DEBUG_LEVEL == DEV
    Serial.begin(115200);
  #endif

  DataModule* selected_data_module = create_data_module_type();

  selected_data_module->data_module_initialization_procedure();
  
  selected_data_module->data_module_operating_procedure();

  
  return 0;
}