/*
 * rcc.h
 *
 *  Created on: Sep 26, 2024
 *      Author: Administrator
 */

#ifndef RCC_H
#define RCC_H

//#include <stdint.h>
#include "stm32f446re.h" // Adjust based on your STM32 series



 //uint32_t SystemCoreClock; // Declare the core clock variable

void SystemCoreClockUpdate(void); // Function prototype
 // Include the system header

// RCC register structure

// RCC base address
//#define RCC ((RCC_RegDef_t*)0x40023800) // Adjust based on your STM32 series

// Function prototypes
void RCC_Init_ExternalClock(void);
//void SysTick_Init(uint32_t ticks);
void Delay(uint32_t ms);

#endif // RCC_H
