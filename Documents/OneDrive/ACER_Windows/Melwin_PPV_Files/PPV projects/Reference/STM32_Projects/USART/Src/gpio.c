#include "gpio.h"

#define RCC    ((RCC_TypeDef *) RCC__PERIPHERAL)
#define GPIOA  ((GPIO_TypeDef *) GPIOA_PERIPHERAL)
#define GPIOB  ((GPIO_TypeDef *) GPIOB_PERIPHERAL)
#define GPIOC  ((GPIO_TypeDef *) GPIOC_PERIPHERAL)
#define USART1 ((USART_TypeDef *) USART1_PERIPHERAL)
#define USART2 ((USART_TypeDef *) USART2_PERIPHERAL)

void rcc_enable_gpio(GPIO_TypeDef *gpio) {
    if (gpio == GPIOA) RCC->AHB1ENR |= (1 << 0);
    else if (gpio == GPIOB) RCC->AHB1ENR |= (1 << 1);
    else if (gpio == GPIOC) RCC->AHB1ENR |= (1 << 2);
}

void rcc_enable_usart1(void) {
    RCC->APB2ENR |= (1 << 4);  // USART1 on APB2
}

void rcc_enable_usart2(void) {
    RCC->APB1ENR |= (1 << 17); // USART2 on APB1
}

void gpio_init_alternate(GPIO_TypeDef *gpio, uint8_t pin, uint8_t af) {
    gpio->MODER &= ~(0x3 << (pin * 2));
    gpio->MODER |=  (0x2 << (pin * 2)); // Alternate function

    if (pin < 8) {
        gpio->AFRL &= ~(0xF << (pin * 4));
        gpio->AFRL |=  (af << (pin * 4));
    } else {
        gpio->AFRH &= ~(0xF << ((pin - 8) * 4));
        gpio->AFRH |=  (af << ((pin - 8) * 4));
    }

    gpio->OSPEEDR |= (0x3 << (pin * 2)); // High speed
    gpio->PUPDR &= ~(0x3 << (pin * 2));  // No pull
}

void gpio_init_output_level(GPIO_TypeDef *gpio, uint8_t pin, uint8_t level) {
    gpio->MODER &= ~(0x3 << (pin * 2));
    gpio->MODER |=  (0x1 << (pin * 2)); // Output mode

    gpio->OTYPER &= ~(1 << pin);        // Push-pull
    gpio->OSPEEDR |= (0x3 << (pin * 2));
    gpio->PUPDR &= ~(0x3 << (pin * 2));

    if (level)
        gpio->ODR |= (1 << pin);
    else
        gpio->ODR &= ~(1 << pin);
}

// USART1 functions
void usart1_init(uint32_t baudrate) {
    USART1->BRR = 16000000 / baudrate;
    USART1->CR1 |= (1 << 2) | (1 << 3) | (1 << 13); // RE, TE, UE
}

void usart1_write_char(char ch) {
    while (!(USART1->SR & (1 << 7)));
    USART1->DR = ch;
}

char usart1_read_char(void) {
    while (!(USART1->SR & (1 << 5)));
    return USART1->DR;
}

void usart1_write_string(const char *str) {
    while (*str) {
        usart1_write_char(*str++);
    }
}

// USART2 functions
void usart2_init(uint32_t baudrate) {
    USART2->BRR = 16000000 / baudrate;
    USART2->CR1 |= (1 << 2) | (1 << 3) | (1 << 13); // RE, TE, UE
}

void usart2_write_char(char ch) {
    while (!(USART2->SR & (1 << 7)));
    USART2->DR = ch;
}

void usart2_write_string(const char *str) {
    while (*str) {
        usart2_write_char(*str++);
    }
}
