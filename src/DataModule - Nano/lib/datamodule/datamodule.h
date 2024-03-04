#ifndef DATAMODULE_H
#define DATAMODULE_H


namespace BAJA_EMBEDDED {
    
    class DataModule {

    protected:
        static const int arraySize = 10; // one data module can record 10 different values on one line at a time
        float dataToRecord[arraySize]; // Static array of 10 floats

        char data_module_type[50];
        virtual void set_data_module_type() = 0;

        virtual void data_module_setup_procedure() = 0;
        virtual void data_module_logging_procedure() = 0;

        void recordDataToSDCard(float time, float data);

    public:
        virtual ~DataModule() {} // Virtual destructor is important for a class with virtual functions
        DataModule();

        
        void data_module_initialization_procedure();
        void data_module_operating_procedure();

        bool waitForCommand(const char* cmmdString);

    };

}

////////////////////////////////////////////////////////////////////////
/////////////////////////initialization stuff/////////////////////////

BAJA_EMBEDDED::DataModule* create_data_module_type();
void initialize_data_module_select_pins();

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/////////////////////////sd card stuff//////////////////////////////////

void InitializeSDCard();

void SendFile();

void StartSDReading();

void CloseSDFile();

////////////////////////////////////////////////////////////////////////

#endif // DATAMODULE_H
