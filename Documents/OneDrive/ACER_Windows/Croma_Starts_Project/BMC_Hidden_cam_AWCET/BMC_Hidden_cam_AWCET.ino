#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// === Global Variables ===
volatile unsigned long millis_count = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 50; // ms
uint8_t State = 0;

// === Function Prototypes ===
void init_pins(void);
void init_timer0(void);
void Buzz(void);
void StopBuzz(void);
uint16_t readADC(uint8_t channel);

// === Main Program ===
int main(void) {
    init_pins();
    init_timer0();

    uint8_t stateInput, irValue;
    uint16_t sensorValue;

    while (1) {
        // Read Mode (D3)
        stateInput = (PIND & (1 << PD3)) ? 1 : 0;
        // Read IR (D5)
        irValue = (PIND & (1 << PD5)) ? 1 : 0;
        // Read LDR (ADC6)
        sensorValue = readADC(6);

        if (stateInput == 1) {
            // === IR MODE ===
            PORTB &= ~(1 << PB2); // D10 LOW
            PORTB |= (1 << PB3);  // D11 HIGH

            if (irValue == 0) {
                Buzz();
            } else {
                StopBuzz();
            }
        } else {
            // === LDR MODE ===
            PORTB |= (1 << PB2);  // D10 HIGH
            PORTB &= ~(1 << PB3); // D11 LOW

            if (sensorValue <= 90) {
                Buzz();
            } else {
                StopBuzz();
            }
        }
    }
}

// === Initialize I/O Pins ===
void init_pins(void) {
    // Inputs: D3 (PD3), D5 (PD5)
    DDRD &= ~((1 << PD3) | (1 << PD5));

    // Outputs: D10 (PB2), D11 (PB3), D12 (PB4), A5 (PC5)
    DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB4);
    DDRC |= (1 << PC5);
}

// === Initialize Timer0 for millis() counter ===
void init_timer0(void) {
    // Timer0 in CTC mode
    TCCR0A = (1 << WGM01);
    // 64 prescaler
    TCCR0B = (1 << CS01) | (1 << CS00);
    // Compare value for 1ms tick (16MHz / 64 / 250 = 1ms)
    OCR0A = 249;
    // Enable compare interrupt
    TIMSK0 = (1 << OCIE0A);
    // Enable global interrupts
    sei();
}

// === Interrupt Service Routine: every 1 ms ===
ISR(TIMER0_COMPA_vect) {
    millis_count++;
}

// === Simulate Arduino millis() ===
unsigned long millis(void) {
    unsigned long m;
    cli();
    m = millis_count;
    sei();
    return m;
}

// === ADC Reading Function ===
uint16_t readADC(uint8_t channel) {
    ADMUX = (1 << REFS0) | (channel & 0x07); // AVCC reference
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1);
    while (ADCSRA & (1 << ADSC));  // Wait for conversion
    return ADC;
}

// === Non-blocking Buzzer Toggle ===
void Buzz(void) {
    unsigned long currentMillis = millis();

    PORTB &= ~(1 << PB3); // D11 LOW
    PORTB &= ~(1 << PB2); // D10 LOW

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        State = !State;

        if (State) {
            PORTC |= (1 << PC5);  // Buzzer ON
            PORTB |= (1 << PB4);  // D12 ON
        } else {
            PORTC &= ~(1 << PC5); // Buzzer OFF
            PORTB &= ~(1 << PB4); // D12 OFF
        }
    }
}

// === Stop Buzzer ===
void StopBuzz(void) {
    PORTC &= ~(1 << PC5);
    PORTB &= ~(1 << PB4);
    State = 0;
}
