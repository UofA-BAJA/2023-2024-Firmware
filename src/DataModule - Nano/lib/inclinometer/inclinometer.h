#ifndef INCLINOMETER_H
#define INCLINOMETER_H

#include <datamodule.h>

class Inclinometer_DataModule : public BAJA_EMBEDDED::DataModule
{
    private:

        // void initialize_inclinometer();
    public:
        void data_module_initialization_procedure() override;
        void data_module_operating_procedure() override;
};

#endif // INCLINOMETER_H