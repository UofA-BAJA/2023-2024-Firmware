#ifndef DATAMODULE_H
#define DATAMODULE_H

#include "config.h"
#include "sdcard.h"

namespace BAJA_EMBEDDED {
    
    class DataModule {


    public:
        virtual ~DataModule() {} // Virtual destructor is important for a class with virtual functions
        DataModule();

        void data_module_initialization_procedure();
        void data_module_operating_procedure();

        virtual void data_module_setup_procedure() = 0; //ive never seen this =0 syntax but thats how u do it
        virtual void data_module_logging_procedure() = 0; //ive never seen this =0 syntax but thats how u do it
        
    };

}

BAJA_EMBEDDED::DataModule* create_data_module_type();

void initialize_data_module_select_pins();

#endif // DATAMODULE_H
