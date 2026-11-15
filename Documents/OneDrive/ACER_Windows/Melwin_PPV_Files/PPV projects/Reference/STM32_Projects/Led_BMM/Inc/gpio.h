#ifndef GPIO_H_
#define GPIO_H_
#include "base.h"

#define GPIO_MODER_OFFSET  (0x00)
#define GPIO_MODER         (*(volatile unsigned int *)(GPIOA__PERIPHERAL + GPIO_MODER_OFFSET))

#define GPIO_OTYPER_OFFSET (0x04)
#define GPIO_OTYPER        (*(volatile unsigned int *)(GPIOA__PERIPHERAL + GPIO_OTYPER_OFFSET))

#define GPIO_ODR_OFFSET    (0x14)
#define GPIO_ODR           (*(volatile unsigned int *)(GPIOA__PERIPHERAL + GPIO_ODR_OFFSET))

#define RCC_AHB1ENR_OFFSET (0x30)
#define RCC_AHB1ENR        (*(volatile unsigned int *)(RCC__PERIPHERAL + RCC_AHB1ENR_OFFSET))



void gpio_init(void);
void delay(long int Time);

#endif /* GPIO_H_ */
