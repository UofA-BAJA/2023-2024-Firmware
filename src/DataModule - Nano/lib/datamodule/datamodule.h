#ifndef DATAMODULE_H
#define DATAMODULE_H

#include <config.h>

namespace BAJA_EMBEDDED {
    
    class DataModule {
       
    public:
        virtual ~DataModule() {} // Virtual destructor is important for a class with virtual functions
        DataModule();

        virtual void data_module_initialization_procedure() = 0; //ive never seen this =0 syntax but thats how u do it
        
        virtual void data_module_operating_procedure() = 0;
    };

}

BAJA_EMBEDDED::DataModule* create_data_module_type();

void initialize_data_module_select_pins();

#endif // DATAMODULE_H
