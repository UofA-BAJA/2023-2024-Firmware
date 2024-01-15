#ifndef IMU_H
#define IMU_H

#include <datamodule.h>

class IMU_DataModule : public BAJA_EMBEDDED::DataModule
{
    private:

        // void initialize_IMU();
    public:
        void data_module_initialization_procedure() override;
        void data_module_operating_procedure() override;
};

#endif // IMU_H