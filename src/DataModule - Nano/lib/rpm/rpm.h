#ifndef RPM_H
#define RPM_H

//note that everything is from the driver's perspective
extern float rear_left_rpm; //real rpm value of the rear left tire
extern int rear_left_rpm_counter; //counter for the rear left tire


void initialize_rpm_sensor();

void start_counting_rpms();

void stop_counting_rpms();

#endif // RPM_H
