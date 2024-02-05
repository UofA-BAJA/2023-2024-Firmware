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

  // not sure why this isn't working
  DataModule* selected_data_module = create_data_module_type();
  // DataModule* selected_data_module = new RPM_DataModule();

  selected_data_module->data_module_initialization_procedure();
  
  selected_data_module->data_module_operating_procedure();

  
  return 0;
}