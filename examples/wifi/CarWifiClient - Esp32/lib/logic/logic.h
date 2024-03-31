#ifndef LOGIC_H
#define LOGIC_H

#include <Arduino.h>

void operatingProcedure();

void establishWirelessConnection();

void parseMessageRecursive(char* message, char* end);
void parseMessage(char* startOfMessage);
#endif // LOGIC_H