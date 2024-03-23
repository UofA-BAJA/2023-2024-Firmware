#ifndef ANALOG_H
#define ANALOG_H

#include <datamodule.h>

class BRK_DataModule : public BAJA_EMBEDDED::DataModule
{
    private:
        ;
    public:
        int analogPin = 5; // ADC pin you're going to read
        void set_data_module_type() override;
        void data_module_setup_procedure() override;
        void data_module_logging_procedure() override;
};

#endif // IMU_H