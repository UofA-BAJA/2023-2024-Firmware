#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>


volatile unsigned long timer2_overflow_counter = 0;
// Initialize Timer2 for overflow interrupt with prescaler 1024
void initTimer2() {
    // Clear Waveform Generation Mode bits for normal operation
    TCCR2A &= ~((1 << WGM21) | (1 << WGM20));
    TCCR2B &= ~(1 << WGM22);
    
    // Enable Timer2 overflow interrupt
    TIMSK2 |= (1 << TOIE2);
    
    sei(); // Enable global interrupts
}

void stopTimer2() {
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20)); // Stop the timer
}

void startTimer2() {
    // Set prescaler to 64
    TCCR2B |= (1 << CS21) | (1 << CS20);
    TCCR2B &= ~(1 << CS22);
    /*
    with a clk of 8Mhz and a prescaler of 64, we get a tick every 125kHz or 0.000008 seconds
    our timer is 8bit so it will overflow at 0.000008 * 2^8 = 0.002048 seconds
    the shortest time between two magnets is 8ms = 0.008 seconds
    so we will still get about 4 overflows at minimum*/
}

unsigned long readMicrosecondsElapsedFromTimer2() {

    // Calculate the time elapsed in seconds
    unsigned long timeElapsed = (timer2_overflow_counter * 2048) + (TCNT2 * 8);
    // Reset the timer
    return timeElapsed;
}

// Timer2 Overflow Interrupt Service Routine
ISR(TIMER2_OVF_vect) {
    timer2_overflow_counter++;
    //clear the overflow flag by writing a 1 to it
    TIFR2 |= (1 << TOV2);
}

