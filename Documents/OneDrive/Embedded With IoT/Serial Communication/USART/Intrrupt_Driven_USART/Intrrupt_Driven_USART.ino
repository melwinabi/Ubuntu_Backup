#include <avr/interrupt.h>

volatile char receivedChar;  // Variable to store received data
volatile bool dataReady = false;

void setup() {
  Serial.begin(9600);          // Initialize USART
  UCSR0B |= (1 << RXCIE0);     // Enable USART RX Complete Interrupt
  sei();                       // Enable global interrupts
}

ISR(USART_RX_vect) {           // USART Receive Complete Interrupt
  receivedChar = UDR0;         // Read data from USART data register
  dataReady = true;            // Indicate data is ready
}

void loop() {
  if (dataReady) {             // Check if data is ready
    Serial.print("Received: ");
    Serial.println(receivedChar);
    dataReady = false;         // Reset flag
  }
}
