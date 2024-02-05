#include <HardwareSerial.h>

#include "datamodule.h"
#include "config.h"

#include "imu.h"
#include "rpm.h"

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