#include <avr/io.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>
#include <SdFat.h>
#include <SPI.h>

#include "datamodule.h"
#include "macros.h"
#include "enums.h"

#include "imu.h"
#include "rpm.h"
#include "analog.h"
///serial stuff
// Define startMarker and endMarker as preprocessor macros
#define startMarker '<'
#define endMarker '>'
const char numChars = 32;
char receivedChars[numChars];   // an array to store the received data
bool newData = false;
////////////////////////////

//sd card stuff
SdFat sd;

// File object.
SdFile file;
// File dataFile;
#define fileName "temp.csv"
#define chipSelect 10
////////////////////////////

//counter stuff
volatile unsigned long BAJA_EMBEDDED::lifetime_overflow_counter = 0;
unsigned long BAJA_EMBEDDED::lifetime_timeElapsed_microseconds = 0;
volatile bool BAJA_EMBEDDED::lifetime_overflow_flag_change = false;
////////////////////////////

//state stuff
enum DataModuleState {
    GENERAL_INITIALIZATION,
    DATAMODULE_SPECIFIC_INITIALIZATION,
    RESPOND_WITH_TYPE,
    WAIT_TO_START_LOGGING,
    LOG_DATA,
    WAIT_TO_SEND_FILE
};

DataModuleState data_module_state = GENERAL_INITIALIZATION; //initial state



BAJA_EMBEDDED::DataModule::DataModule() {
    sei();

    //initialize the dataToRecord array to have -1 values as default
    for (int i = 0; i < arraySize; ++i) {
        dataToRecord[i] = -1.0f; // Assuming -1.0f is never a valid data point
    }

    //initialize the dataHeaderArray to have "EMPTY" as default
    for(int i = 0; i < arraySize; ++i) {
        strcpy(dataHeaderArray[i], "EMPTY");
    }
}

void BAJA_EMBEDDED::DataModule::data_module_operating_procedure() {
    while(1) {
        switch (data_module_state)
        {
        case GENERAL_INITIALIZATION:
            initLifetimeTimer();
            DEBUG_PRINTLN("Init sd card...");
            InitializeSDCard();
            data_module_state = DATAMODULE_SPECIFIC_INITIALIZATION;
            DEBUG_PRINTLN("Finished General Init");
            break;
        

        case DATAMODULE_SPECIFIC_INITIALIZATION:
            data_module_setup_procedure();
            set_data_module_type();
            SetupFileAsCSV();
            data_module_state = RESPOND_WITH_TYPE;
            DEBUG_PRINTLN("Ready");
            break;
        

        case RESPOND_WITH_TYPE:
            if (waitForCommand(COMMANDS_SENDTYPE)) {
                Serial.println(data_module_type);
                Serial.flush();
                
                data_module_state = WAIT_TO_START_LOGGING;
            } 
            break;


        case WAIT_TO_START_LOGGING:
            
            if (waitForCommand(COMMANDS_BEGIN)) {
                resetLifetimeTimer();
                startLifetimeTimer();
                StartSDReading();
                data_module_state = LOG_DATA;
                DEBUG_PRINTLN("Started data logging");
            }
            break;


        case LOG_DATA:

            if (Serial.available() > 0) {
                if (waitForCommand(COMMANDS_END)) {
                    stopLifetimeTimer();
                    CloseSDFile();
                    data_module_state = WAIT_TO_SEND_FILE;
                    DEBUG_PRINTLN("Stopped data logging...");
                }
            }
            else {
                data_module_logging_procedure();
            }
            break;


        case WAIT_TO_SEND_FILE:
            
            if (waitForCommand(COMMANDS_RETRIEVE)) { 
                SendFile();
                deleteFile();
                SetupFileAsCSV();
                data_module_state = WAIT_TO_START_LOGGING;
            }
           
            break;
        }

    _delay_us(10); //delay for 10us
    }


}
////////////////////////////////////////////////////////////////////////
/////////////////////////initialization stuff/////////////////////////

BAJA_EMBEDDED::DataModule* create_data_module_type() {
    
    initialize_data_module_select_pins();
    // DEBUG_PRINTLN("Data Module Select Pins Initialized");
    //read the select pins
    int data_module_select =((PINC & (1 << PINC2)) >> PINC2) << 2 | 
                            ((PINC & (1 << PINC1)) >> PINC1) << 1 | 
                            ((PINC & (1 << PINC0)) >> PINC0);

    // DEBUG_PRINT("Data Module Select Pin Reads: ");
    // DEBUG_PRINTLN(data_module_select);

    if (data_module_select == 0b111) {
        DEBUG_PRINTLN("RPM Module Detected");
        return new RPM_DataModule;
    }
    else if(data_module_select == 0b110){
        DEBUG_PRINTLN("IMU Detected");
        return new IMU_DataModule;
    }
    else if(data_module_select == 0b101) {
        return new BRK_DataModule;
    }
    else {
        DEBUG_PRINTLN("No datamodule found for this pin selection");
            
        return nullptr; 
    }

}

void initialize_data_module_select_pins() {
    //look at pinout csv files
    // Clear the bits DDC0, DDC1, and DDC2 in the Data Direction Register C (DDRC).
    // This configures the pins PC0, PC1, and PC2 as inputs.
    DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC2));
        
    // Set the bits PORTC0, PORTC1, and PORTC2 in the Port C Data Register (PORTC).
    // This enables the internal pull-up resistors for the pins PC0, PC1, and PC2.
    PORTC |= ( (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2));
}

////////////////////////////////////////////////////////////////////////
/////////////////////////serial communication/////////////////////////
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

/*this function will wait for a command to be received and will return a boolean if the command is successfuly recieved
    give the function a defined command from enums.h as input*/
bool BAJA_EMBEDDED::DataModule::waitForCommand(const char* cmmdString) {
    
    recvWithStartEndMarkers();
    if (newData) {

        if (strcmp(receivedChars, cmmdString) == 0) {
            receivedChars[0] = '\0'; //clear received chars
            return true;
        }
        else if (receivedChars[0] != '\0')
        {
            DEBUG_PRINT("DataModule is in state: ");
            DEBUG_PRINTLN(data_module_state);
            DEBUG_PRINT("Incorrect Command: ");
            DEBUG_PRINTLN(receivedChars);
        }
        else {
            // The receivedChars array is empty, so do not print "command not recognized"
        }

        newData = false;
    }
    

    return false;
}

////////////////////////////////////////////////////////////////////////
///////////////////////////////lifetime timer///////////////////////////
void initLifetimeTimer() {
    // Clear Waveform Generation Mode bits for normal operation
    TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0));

    TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
    TCCR1B &= ~(1 << WGM12);

    TIMSK1 |= (1 << TOIE1); // Enable Timer1 overflow interrupt
}

void startLifetimeTimer() {
    // Set prescaler to 1024 and starting the timer
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TCCR1B &= ~(1 << CS11);
}

void stopLifetimeTimer() {
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10)); // Stop the timer
}

void resetLifetimeTimer() {
    BAJA_EMBEDDED::lifetime_overflow_counter = 0;

    // Reset TCNT1 to 0; for demonstration, we'll use the method applicable for general values
    uint16_t value = 0; // Assuming you want to reset the timer to 0
    
    // Split the 16-bit value into two 8-bit values and write them to TCNT1H and TCNT1L
    TCNT1H = (unsigned char)(value >> 8); // Write high byte
    TCNT1L = (unsigned char)(value & 0xFF); // Write low byte

}

unsigned long readMicrosecondsLifetimeTimer() {
    cli(); //disable interrupts
    unsigned int high = TCNT1H;
    unsigned int low = TCNT1L;
    unsigned long timer_count = (high << 8) | low;
    unsigned long current_overflow_count = BAJA_EMBEDDED::lifetime_overflow_counter;
    sei(); //enable 

    if (BAJA_EMBEDDED::lifetime_overflow_flag_change) {
        BAJA_EMBEDDED::lifetime_timeElapsed_microseconds = (current_overflow_count - 1) * 8388608; //8388608 = (2^16) * 128
        BAJA_EMBEDDED::lifetime_overflow_flag_change = false;
    }
    else {
        BAJA_EMBEDDED::lifetime_timeElapsed_microseconds = current_overflow_count * 8388608;
    }
    BAJA_EMBEDDED::lifetime_timeElapsed_microseconds += timer_count * 128;

    // DEBUG_PRINT(">counter: ");
    // DEBUG_PRINTLN(timer_count);
    // DEBUG_PRINT(">ovf_count: ");
    // DEBUG_PRINTLN(current_overflow_count);
    // DEBUG_PRINT(">time: ");
    // DEBUG_PRINTLN(BAJA_EMBEDDED::lifetime_timeElapsed_microseconds);

    return BAJA_EMBEDDED::lifetime_timeElapsed_microseconds;
}


ISR(TIMER1_OVF_vect) {
    // Timer1 overflow interrupt service routine
    // This ISR is called whenever Timer1 overflows
    // The timer overflows when TCNT1 goes from 0xFFFF to 0x0000
    // This ISR is called every 8.388608 seconds
    // Increment the counter
    BAJA_EMBEDDED::lifetime_overflow_counter++;
    BAJA_EMBEDDED::lifetime_overflow_flag_change = true;
    TIFR1 |= (1 << TOV1); // Clear the overflow flag by writing a 1 to it
}

////////////////////////////////////////////////////////////////////////
/////////////////////////sd card stuff//////////////////////////////////
void InitializeSDCard(){
    
    // if(!SD.begin(chipSelect)){
    //     DEBUG_PRINTLN("Card failed, or not present");
    //     while(1);
    // } 
    if (!sd.begin(chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("SD Card initialization failed!");
    return;
  }

    // if (sd.exists(fileName)) {
    // // Serial.println("File exists. Removing...");
    //     if (sd.remove(fileName)) {
    //     // Serial.println("File removed successfully.");
    //     } else {
    //     // Serial.println("File removal failed.");
    //     }
    // } else {
    //     // Serial.println("File does not exist.");
    // }

    deleteFile();

}

void deleteFile(){
    if (sd.exists(fileName)) {
        Serial.println("File exists. Removing...");
        if (sd.remove(fileName)) {
        Serial.println("File removed successfully.");
        } else {
        Serial.println("File removal failed.");
        }
    } else {
        Serial.println("File does not exist.");
    }
}

void SendFile(){

      // Open the file for reading
    if (!file.open(fileName, O_READ)) {
        Serial.println("Failed to open file for reading.");
        return;
    }

    // Read from the file until end of file
    char ch;
    while (file.read(&ch, 1) > 0) {
        Serial.write(ch);
    }

    // Close the file
    file.close();
    Serial.println("<Finished>");
    Serial.flush();
}



void StartSDReading() {
    
    if (!file.open(fileName, O_CREAT | O_WRITE | O_APPEND)) {
        Serial.println("Opening file failed!");
        return;
    }
    
    // _delay_ms(100); //delay for 100ms

    // if (!dataFile) {
    //     DEBUG_PRINTLN("Failed to open file for writing, trying  again...");

    //     dataFile = SD.open(fileName, FILE_WRITE);

    //     if (!dataFile) {
    //         DEBUG_PRINTLN("Failed to open file again");
    //         while(1);
    //      }
    // }
}

void BAJA_EMBEDDED::DataModule::SetupFileAsCSV() {
    if (!file.open(fileName, O_CREAT | O_WRITE | O_APPEND)) {
        DEBUG_PRINTLN("Opening file failed!");
        return;
    }

    file.print("Micros,");

    for (int i = 0; i < arraySize; ++i) {
        // Use strcmp to compare strings. If the result is not 0, the strings are not equal.
        if (strcmp(dataHeaderArray[i], "EMPTY") != 0) {
            file.print(dataHeaderArray[i]);
            file.print(",");
        }

        if (i == arraySize - 1) {
            file.println();
        }
    }

    file.close();

}

void BAJA_EMBEDDED::DataModule::recordDataToSDCard(){

    file.print(micros());

    for (int i = 0; i < arraySize; ++i) {
        if (dataToRecord[i] != -1.0f) { // Check against sentinel value
            // Write data[i] to the SD card
            
            file.print(",");
            file.print(dataToRecord[i], 4);
        }

        if (i == arraySize - 1) {
            file.println();
        }
    }
    
}


void CloseSDFile(){
    file.close();
}