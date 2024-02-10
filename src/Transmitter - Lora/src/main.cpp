
#include "LoRaWan_APP.h" // Include LoRaWAN application-specific header
#include "Arduino.h"     // Include Arduino framework header

/*
 * set LoraWan_RGB to 1, the RGB active in LoRaWAN
 * RGB red means sending;
 * RGB green means received done;
 */
#ifndef LoraWan_RGB
#define LoraWan_RGB 0
#endif

#define RF_FREQUENCY                                868000000 // Frequency in Hz

#define TX_OUTPUT_POWER                             5        // Transmit power in dBm

#define LORA_BANDWIDTH                              0        // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR                       7        // [SF7..SF12]
#define LORA_CODINGRATE                             1        // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8        // Preamble length for both Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0        // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false    // Fixed-length payload
#define LORA_IQ_INVERSION_ON                        false    // Inversion of IQ signals

#define RX_TIMEOUT_VALUE                            1000     // Timeout value for receiving
#define BUFFER_SIZE                                 30       // Define the payload size here

char txpacket[BUFFER_SIZE]; // Transmission packet buffer
char rxpacket[BUFFER_SIZE]; // Reception packet buffer

static RadioEvents_t RadioEvents; // Struct to hold radio event functions
void OnTxDone( void );            // Function called on transmission completion
void OnTxTimeout( void );         // Function called on transmission timeout
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ); // Function called on reception
String userInput; // Declare userInput outside the switch
unsigned long int mytime=0;

long int diff;
typedef enum
{
    LOWPOWER,
    RX,
    TX
} States_t; // Enumeration for device states

int16_t txNumber;    // Transmission number
States_t state;      // Current device state
bool sleepMode = false; // Device sleep mode
int16_t Rssi, rxSize;   // Received signal strength indicator (RSSI) and received size
void DoubleToString(char *str, double double_num, unsigned int len);
bool indication = true;
bool unsent;

void setup() {
    Serial.begin(115200); // Initialize serial communication

    txNumber = 0; // Initialize transmission number
    Rssi = 0;     // Initialize RSSI

    RadioEvents.TxDone = OnTxDone;       // Set function callbacks for radio events
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;

    Radio.Init(&RadioEvents); // Initialize the radio module
    Radio.SetChannel(RF_FREQUENCY); // Set radio channel frequency
    // Set transmission configuration parameters
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
    // Set reception configuration parameters
    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                      LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
    state = TX; // Set the initial state to TX
}

void loop() {
    if(indication){
        turnOnRGB(COLOR_SEND,0);
        delay(2000);
        turnOnRGB(0,0);
        indication = false;
    }
    switch(state) {
        case TX:
            delay(100);
            txNumber += 0.01;
            
            while(Serial.available() == 0);
            
            // Read the input word until a newline character is encountered
            userInput = Serial.readStringUntil('\n');

            userInput.trim(); // Remove leading and trailing whitespaces

            // Build the txpacket
            sprintf(txpacket,"%s", userInput.c_str());  //start a package
            // sprintf(txpacket+strlen(txpacket),"%d",txNumber); //add to the end of package
            
            
            turnOnRGB(COLOR_SEND,0); //change rgb color

            Serial.printf("\r\nCommand sent:\"%s\"\r\n",txpacket);

            Radio.Send( (uint8_t *)txpacket, strlen(txpacket) ); //send the package out
            state = LOWPOWER; // Change state to LOWPOWER
            break;

        case RX:
    // Enter reception mode
        Radio.Rx(0);
        // Wait for reception or timeout
        while (unsent) {
        Radio.IrqProcess();
        delay(10);
        if (millis() - mytime > 2000) {
            state = TX;
            Serial.println("Transmission not confirmed, Check range and try again");
            unsent =false;
            }
        }
    break;


        case LOWPOWER:
            lowPowerHandler();
            break;

        default:
            break;
    }
    Radio.IrqProcess(); // Process radio interrupts
}

void OnTxDone(void) {
    turnOnRGB(0, 0); // Turn off RGB LED
    mytime = millis();
    unsent = true;
    state = RX; // Change state to RX
}

void OnTxTimeout(void) {
    Radio.Sleep(); // Put the radio to sleep on transmission timeout
    state = TX; // Change state to TX
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
    Rssi = rssi; // Update RSSI value
    rxSize = size; // Update received size
    memcpy(rxpacket, payload, size); // Copy received payload to buffer
    rxpacket[size] = '\0'; // Null- terminate the received packet
    turnOnRGB(COLOR_RECEIVED, 0); // Function to turn on RGB LED for reception
    Serial.printf("\r\n%s\r\n",rxpacket);
    Radio.Sleep(); // Put the radio to sleep
    unsent = false;
    state = TX; // Change state to TX
}