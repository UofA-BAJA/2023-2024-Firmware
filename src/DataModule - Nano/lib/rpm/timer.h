#ifndef TIMER_H
#define TIMER_H

volatile extern unsigned long timer2_overflow_counter;

void initTimer2();

void stopTimer2();

void startTimer2();

unsigned long readMicrosecondsElapsedFromTimer2();

#endif // TIMER_H