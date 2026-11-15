#include "gpio.h"
#include <string.h>

#define GPIOA ((GPIO_TypeDef *) GPIOA_PERIPHERAL)
#define USART1_AVAILABLE ((USART_TypeDef *) USART1_PERIPHERAL)

int main(void) {
    rcc_enable_gpio(GPIOA);
    rcc_enable_usart1();
    rcc_enable_usart2();  // ✅ Add RCC enable for USART2

    gpio_init_alternate(GPIOA, 9, 7);  // USART1_TX
    gpio_init_alternate(GPIOA, 10, 7); // USART1_RX

    gpio_init_alternate(GPIOA, 2, 7);  // USART2_TX
    gpio_init_alternate(GPIOA, 3, 7);  // USART2_RX

    gpio_init_output_level(GPIOA, 5, 0); // PA5 output default low

    usart1_init(9600);  // Init USART1
    usart2_init(9600);  // Init USART2

    char buffer[100];
    int index = 0;

    usart1_write_string("USART1 Ready\r\n");
    usart2_write_string("USART2 Ready\r\n");

    while (1) {
        if (USART1_AVAILABLE->SR & (1 << 5)) {  // RXNE flag
            char c = usart1_read_char();

            if (index < 99 && c != '\r') {
                buffer[index++] = c;
            } else {
                buffer[index] = '\0';  // Null-terminate

                usart1_write_string("You typed: ");
                usart1_write_string(buffer);
                usart1_write_string("\r\n");

                usart2_write_string(buffer);
                usart2_write_string("\r\n");

                if (strcmp(buffer, "ON") == 0) {
                    gpio_init_output_level(GPIOA, 5, 1);
                } else {
                    gpio_init_output_level(GPIOA, 5, 0);
                }

                index = 0;
                memset(buffer, 0, sizeof(buffer));  // Clear buffer
            }
        }
    }
}
