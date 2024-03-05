#ifndef RPM_H
#define RPM_H

#include <avr/interrupt.h>

#include "datamodule.h"

// note that everything is from the driver's perspective
class RPM_DataModule : public BAJA_EMBEDDED::DataModule {

    private:
        void initialize_left_rpm_sensor();
        void initialize_right_rpm_sensor();

    public:

        static volatile float leftSpeed;
        static volatile float rightSpeed;           // volatile float RPM_DataModule::speed = 0; // Define and initialize the static member

        float calculate_rpm(float avg_rpm_count);
        float calculate_speed(float rpm);

        void data_module_setup_procedure() override;
        void data_module_logging_procedure() override;

        void set_data_module_type() override;

        static void shift_data_array_left(int *data_array, int array_size);
        static float get_average_of_data_array(int *data_array, int array_size);
};

#endif // RPM_H
