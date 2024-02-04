#ifndef DATAMODULE_H
#define DATAMODULE_H

#include <config.h>
#include <SD.h> // This include is so I can use the File type for the dataFile.

namespace BAJA_EMBEDDED {
    
    class DataModule {
    protected:
        void StartSDReading();
        void InitializeSDReading(int chipSelect, String fileName);
        void WriteToSD(String dataString);
        void CloseSD();

        // Maybe change the return type to a bool to see if the file was successfully sent
        void SendFile();

    private:
        int chipSelect = 10; // Where the CS Pin is plugged into
        String fileName = "data.txt";
        File dataFile;

    public:
        virtual ~DataModule() {} // Virtual destructor is important for a class with virtual functions
        DataModule();

        void data_module_initialization_procedure();
        void data_module_operating_procedure();

        virtual void data_module_specific_initialization_procedure() = 0; //ive never seen this =0 syntax but thats how u do it
        
        virtual void data_module_specific_operating_procedure() = 0;
    };

}

BAJA_EMBEDDED::DataModule* create_data_module_type();

void initialize_data_module_select_pins();

#endif // DATAMODULE_H
