#include "datamodule.h"
#include "config.h"

using namespace BAJA_EMBEDDED;

int main() {
  setup_debug();

  DataModule* selected_data_module = create_data_module_type();
  
  selected_data_module->data_module_initialization_procedure();
  
  selected_data_module->data_module_operating_procedure();

  
  return 0;
}