#ifndef DATAMODULE_H
#define DATAMODULE_H

enum class DataModuleType {
    RPM //0b001
};

class DataModule {
    private:
        int error_code;
        DataModuleType type;
       
    public:
        DataModule();

        void self_identify_type_of_data_module();

        void data_module_initialization_procedure();
        
        void data_module_operating_procedure();
};

#endif // DATAMODULE_H
