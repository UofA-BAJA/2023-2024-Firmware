#ifndef DATAMODULE_H
#define DATAMODULE_H



namespace BAJA_EMBEDDED {
    

    class DataModule {

    protected:
        static const int arraySize = 5; // one data module can record 5 different values on one line at a time
        float dataToRecord[arraySize]; // Static array of 10 floats

        char dataHeaderArray[arraySize][15]; //this is the header row on the csv file, max 15 chars per column

        char data_module_type[50];
        virtual void set_data_module_type() = 0;

        virtual void data_module_setup_procedure() = 0;
        virtual void data_module_logging_procedure() = 0;

        void recordDataToSDCard();
        void SetupFileAsCSV();

    public:
        virtual ~DataModule() {} // Virtual destructor is important for a class with virtual functions
        DataModule();

        
        void data_module_initialization_procedure();
        void data_module_operating_procedure();

        bool waitForCommand(const char* cmmdString);

    };

    extern volatile unsigned long lifetime_overflow_counter;
    extern unsigned long lifetime_timeElapsed_microseconds;
    extern volatile bool lifetime_overflow_flag_change;
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
////////////////////////////lifetime counter////////////////////////////

void initLifetimeTimer();

void startLifetimeTimer();

void stopLifetimeTimer();

void resetLifetimeTimer();

unsigned long readMicrosecondsLifetimeTimer();

#endif // DATAMODULE_H
