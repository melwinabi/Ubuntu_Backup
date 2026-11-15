#include "gpio.h"

#define GPIOA5 (1 << 5)

int main(void)
{
	gpio_init();
	for(;;)
	{
		GPIO_ODR ^= GPIOA5;
		delay(10000000);
	}
}

void delay(long int Time)
{
	while(Time --);
}
