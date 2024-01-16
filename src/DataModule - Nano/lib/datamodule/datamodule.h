#ifndef DATAMODULE_H
#define DATAMODULE_H

#include <config.h>
#include <SPI.h>
#include <SD.h>

namespace BAJA_EMBEDDED {
    
    class DataModule {
    protected:
        void StartSDReading();
        void InitializeSDReading(int chipSelect, String fileName);
        void WriteToSD(String dataString);
        void CloseSD();

    private:
        int chipSelect = 10; // Where the CS Pin is plugged into
        String fileName;
        File dataFile;
    public:
        virtual ~DataModule() {} // Virtual destructor is important for a class with virtual functions
        DataModule();
        DataModule(int chipSelect, String fileName);

        virtual void data_module_initialization_procedure() = 0; //ive never seen this =0 syntax but thats how u do it
        
        virtual void data_module_operating_procedure() = 0;
    };

}

BAJA_EMBEDDED::DataModule* create_data_module_type();

void initialize_data_module_select_pins();

#endif // DATAMODULE_H
