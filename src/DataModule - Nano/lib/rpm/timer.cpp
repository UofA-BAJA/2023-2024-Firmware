#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>


int timer0_overflow_count = 0;
// Initialize Timer0 for overflow interrupt with prescaler 1024
void initTimer0() {
    // Clear Waveform Generation Mode bits for normal operation
    TCCR0A &= ~((1 << WGM01) | (1 << WGM00));
    TCCR0B &= ~(1 << WGM02);
    
    // Enable Timer0 overflow interrupt
    TIMSK0 |= (1 << TOIE0);
    
    sei(); // Enable global interrupts
}

void stopTimer0() {
    TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00)); // Stop the timer
}

void startTimer0() {
    // Set prescaler to 64
    TCCR0B |= (1 << CS01) | (1 << CS00);
    TCCR0B &= ~(1 << CS02);
    /*
    with a clk of 8Mhz and a prescaler of 64, we get a tick every 125kHz or 0.000008 seconds
    our timer is 8bit so it will overflow at 0.000008 * 2^8 = 0.002048 seconds
    the shortest time between two magnets is 8ms = 0.008 seconds
    so we will still get about 4 overflows at minimum*/
}

// Timer0 Overflow Interrupt Service Routine
ISR(TIMER0_OVF_vect) {
    timer0_overflow_count++;
    //clear the overflow flag by writing a 1 to it
    TIFR0 |= (1 << TOV0);
}

float msTimeElapsedFromTimer0() {

    // Calculate the time elapsed in seconds
    float timeElapsed = (timer0_overflow_count * 2.048) + (TCNT0 * 0.008);
    // Reset the timer
    TCNT0 = 0;
    timer0_overflow_count = 0;

    return timeElapsed;
}