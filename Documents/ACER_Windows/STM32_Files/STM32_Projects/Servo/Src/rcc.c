#include "base.h"
#include "rcc.h"

uint32_t SystemCoreClock = 8000000; // Initialize with default value
#define HSI_VALUE    16000000U // HSI frequency (16 MHz)
#define HSE_VALUE    8000000U  // HSE frequency (8 MHz)

// Function prototypes
void RCC_Init_ExternalClock(void)
{
	RCC -> CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));   // Wait until HSE is stable
	//PLL CONFIG
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk | RCC_PLLCFGR_PLLN_Msk | RCC_PLLCFGR_PLLP_Msk);
    RCC->PLLCFGR |= (8 << RCC_PLLCFGR_PLLM_Pos);  // Set PLLM to 8 (HSE / 8)
    RCC->PLLCFGR |= (192 << RCC_PLLCFGR_PLLN_Pos); // Set PLLN to 144
    RCC->PLLCFGR |= (0 << RCC_PLLCFGR_PLLP_Pos);  // Set PLLP to 2 (divides by 4)

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;       // Set HSE as PLL source

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));  // Wait until PLL is stable

    // Step 4: Configure Flash latency for high clock frequency
    FLASH->ACR |= FLASH_ACR_LATENCY_2WS;  // Set 2 wait states for 72 MHz

    // Step 5: Select PLL as system clock
    RCC->CFGR &= RCC_CFGR_SW_CLEAR;    // Clear SW bits
    RCC->CFGR |= RCC_CFGR_SW_PLL;      // Select PLL as system clock
}
