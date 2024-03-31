#include <Arduino.h>

#include "datamodule.h"

using namespace BAJA_EMBEDDED;


void setup() {

  Serial.begin(115200);

  DataModule* selected_data_module = create_data_module_type();
  
  selected_data_module->data_module_operating_procedure();
}

void loop(void) {}