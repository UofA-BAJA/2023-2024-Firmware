/* Heltec Automation Ping Pong communication test example
 *
 * Function:
 * 1. Ping Pong communication in two CubeCell devices.
 * 
 * Description:
 * 1. Only hardware layer communicates, no LoRaWAN protocol support;
 * 2. Download the same code into two CubeCell devices, then they will begin Ping Pong test each other;
 * 3. This example is for CubeCell hardware basic test.
 *
 * HelTec AutoMation, Chengdu, China
 * 成都惠利特自动化科技有限公司
 * www.heltec.org
 *
 * this project also releases on GitHub:
 * https://github.com/HelTecAutomation/ASR650x-Arduino
 * */
#include <string>

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
String checkInput; // Declare userInput outside the switch

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
bool sendshit = false;
bool indication = true;

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
    state = RX; // Set the initial state to TX
}

void loop() {
if (indication) {
    while(Serial.available() == 0);
    checkInput = Serial.readStringUntil('\n');
    checkInput.trim();
    if (checkInput == "SEND_TYPE"){
        Serial.printf("LORA_PI");
        turnOnRGB(COLOR_RECEIVED,0);
        delay(2000);
        turnOnRGB(0,0);
    }
    indication = false;
}
    switch(state) {
        case TX:
            delay(100); // Delay before each transmission
            txNumber++; // Increment transmission number
            sprintf(txpacket, "%s", "Recieved Command ");
            
            if(sendshit){
                sprintf(txpacket + strlen(txpacket), "%s", "SD info: 00000000");
                sendshit = false;
            }
            sprintf(txpacket + strlen(txpacket), "%s", " Signal Strength: ");
            sprintf(txpacket + strlen(txpacket), "%d", Rssi);
            turnOnRGB(COLOR_SEND, 0); // Function to turn on RGB LED for sending
            

            Radio.Send((uint8_t *)txpacket, strlen(txpacket)); // Send the packet
            state = LOWPOWER; // Change state to LOWPOWER
            break;

        case RX:
            Radio.Rx(0); // Enter reception mode
            state = LOWPOWER; // Chaxnge state to LOWPOWER
            break;

        case LOWPOWER:
            lowPowerHandler(); // Function to handle low power mode
            break;

        default:
            break;
    }
    Radio.IrqProcess(); // Process radio interrupts
}

void OnTxDone(void) {
    turnOnRGB(0, 0); // Turn off RGB LED
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

    const char* condition = "sd read"; // Set your condition C-style string
    std::string rxpacketStr(rxpacket); // Create an std::string from rxpacket

    if (rxpacketStr.compare(condition) == 0) {
    turnOnRGB(200, 1000); // Function to turn on RGB LED for reception
    sendshit = true;
    turnOnRGB(COLOR_RECEIVED, 0); // Function to turn on RGB LED for reception
}
    Serial.printf("\r\n%s\r\n",rxpacket);

    Radio.Sleep(); // Put the radio to sleep
    state = TX; // Change state to TX
}
