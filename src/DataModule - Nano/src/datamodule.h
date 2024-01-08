#ifndef DATAMODULE_H
#define DATAMODULE_H

#include <config.h>

enum class DataModuleType {
    RPM //0b001
};

namespace BAJA_EMBEDDED {
    class DataModule {
    private:
        int error_code;
        BAJA_EMBEDDED::DEBUG_LEVEL debug_level;
        DataModuleType type;
       
    public:
        DataModule();
        DataModule(BAJA_EMBEDDED::DEBUG_LEVEL debug);

        void self_identify_type_of_data_module();

        virtual void data_module_initialization_procedure();
        
        virtual void data_module_operating_procedure();
    };
}


#endif // DATAMODULE_H
