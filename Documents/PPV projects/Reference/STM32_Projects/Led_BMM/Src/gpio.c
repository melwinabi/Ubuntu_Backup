#include "gpio.h"

void gpio_init(void)
{
	RCC_AHB1ENR |= (1 << 0); // enable clock

	// Set Mode  pin A5 - 01 out mode (11 10)
	GPIO_MODER &= ~(1 << 11);
	GPIO_MODER |=  (1 << 10);

	//GPIO_ODR
}
