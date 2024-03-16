#ifndef LOGIC_H
#define LOGIC_H

#include <Arduino.h>

void operatingProcedure();

bool waitForCommand(const char* cmmdString);
void recvWithStartEndMarkers();

String convertToCommand(char* receivedChars);
#endif // LOGIC_H