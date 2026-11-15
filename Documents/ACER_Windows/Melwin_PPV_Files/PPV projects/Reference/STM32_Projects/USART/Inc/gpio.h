#ifndef GPIO_H_
#define GPIO_H_

#include "base.h"

void gpio_init_alternate(GPIO_TypeDef *gpio, uint8_t pin, uint8_t af);
void gpio_init_output(GPIO_TypeDef *gpio, uint8_t pin);
void gpio_init_output_level(GPIO_TypeDef *gpio, uint8_t pin, uint8_t level);

void rcc_enable_gpio(GPIO_TypeDef *gpio);
void rcc_enable_usart1(void);
void rcc_enable_usart2(void);

void usart1_init(uint32_t baudrate);
void usart1_write_char(char ch);
char usart1_read_char(void);
void usart1_write_string(const char *str);

void usart2_init(uint32_t baudrate);
void usart2_write_char(char ch);
void usart2_write_string(const char *str);

#endif /* GPIO_H_ */
