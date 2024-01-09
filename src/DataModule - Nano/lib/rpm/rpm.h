#ifndef RPM_H
#define RPM_H

#include <avr/interrupt.h>

#include "datamodule.h"

//note that everything is from the driver's perspective
class RPM_DataModule : public BAJA_EMBEDDED::DataModule {

    private:
        void initialize_left_rpm_sensor();
    public:
        static volatile int right_rpm_counter;
        static volatile int left_rpm_counter;

        float left_rpm;

        void data_module_initialization_procedure() override;
        void data_module_operating_procedure() override;

        static void shift_data_array_left(int* data_array, int array_size);
        static float get_average_of_data_array(int* data_array, int array_size);

};


#endif // RPM_H
