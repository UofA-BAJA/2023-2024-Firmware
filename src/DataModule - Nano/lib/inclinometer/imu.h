#ifndef IMU_H
#define IMU_H

#include <datamodule.h>

class IMU_DataModule : public BAJA_EMBEDDED::DataModule
{
    private:
        ;
    public:
        void data_module_specific_initialization_procedure() override;
        void data_module_specific_operating_procedure() override;
};

#endif // IMU_H