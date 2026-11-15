/*
 * base.h
 *
 *  Created on: Jul 20, 2025
 *      Author: hardw
 */

#ifndef BASE_H_
#define BASE_H_

#include "stdint.h"

// Base addresses of peripheral registers
#define PERIPH_BASE        0x40000000U
#define AHB1PERIPH_BASE    (PERIPH_BASE + 0x00020000U)
#define APB1PERIPH_BASE    (PERIPH_BASE + 0x00000000U) // APB1 Peripheral Base Address
#define FLASH_BASE         (PERIPH_BASE + 0x00023C00U) // Flash memory base address
#define RCC_BASE           (PERIPH_BASE + 0x00023800U) // RCC Base address
#define SYSTICK_BASE       (PERIPH_BASE + 0xE000E010U) // SysTick Base address

#define GPIOA_BASE (0x40020000U)
#define GPIOB_BASE (0x40020400U)
#define GPIOC_BASE (0x40020800U)
#define GPIOD_BASE (0x40020C00U)
#define GPIOE_BASE (0x40021000U)
#define GPIOF_BASE (0x40021400U)
#define GPIOG_BASE (0x40021800U)
#define GPIOH_BASE (0x40021C00U)

//rcc
#define RCC_CFGR_SWS_HSI                  (0x00000000U)  // HSI oscillator used as the system clock
#define RCC_CFGR_SWS_HSE                  (0x00000004U)  // HSE oscillator used as the system clock
#define RCC_CFGR_SWS_PLL                  (0x00000008U)  // PLL used as the system clock

#define RCC_PLLCFGR_PLLM_Msk      (0x3F)           // Mask for PLLM (6 bits)
#define RCC_PLLCFGR_PLLN_Msk      (0x1FF << 6)     // Mask for PLLN (9 bits, shifted by 6)
#define RCC_PLLCFGR_PLLP_Msk      (0x3 << 16)      // Mask for PLLP (2 bits, shifted by 16)





//all type ... from scratch
typedef struct {
    volatile uint32_t MODER;    // GPIO port mode register
    volatile uint32_t OTYPER;   // GPIO port output type register
    volatile uint32_t OSPEEDR;  // GPIO port output speed register
    volatile uint32_t PUPDR;    // GPIO port pull-up/pull-down register
    volatile uint32_t IDR;      // GPIO port input data register
    volatile uint32_t ODR;      // GPIO port output data register
    volatile uint32_t BSRR;     // GPIO port bit set/reset register
    volatile uint32_t LCKR;     // GPIO port configuration lock register
    volatile uint32_t AFR[2];   // GPIO port alternate function registers
}GPIO_RegDef_t;

// Register map for FLASH
typedef struct {
    volatile uint32_t ACR;      // Flash Access Control Register (0x00)
    volatile uint32_t KEYR;     // Flash Key Register (0x04)
    volatile uint32_t OPTKEYR;  // Flash Option Key Register (0x08)
    volatile uint32_t SR;       // Flash Status Register (0x0C)
    volatile uint32_t CR;       // Flash Control Register (0x10)
    volatile uint32_t ECCR;     // Flash ECC Register (0x14)
    volatile uint32_t OPTR;     // Flash Option Register (0x18)
} FLASH_RegDef_t;


// Register map for RCC

typedef struct {
    volatile uint32_t CR;         // RCC clock control register
    volatile uint32_t PLLCFGR;    // RCC PLL configuration register
    volatile uint32_t CFGR;       // RCC clock configuration register
    volatile uint32_t CIR;        // RCC clock interrupt register
    volatile uint32_t AHB1RSTR;   // RCC AHB1 peripheral reset register
    volatile uint32_t AHB2RSTR;   // RCC AHB2 peripheral reset register
    volatile uint32_t AHB3RSTR;   // RCC AHB3 peripheral reset register
    uint32_t RESERVED0;           // Reserved
    volatile uint32_t APB1RSTR;   // RCC APB1 peripheral reset register
    volatile uint32_t APB2RSTR;   // RCC APB2 peripheral reset register
    uint32_t RESERVED1[2];        // Reserved
    volatile uint32_t AHB1ENR;    // RCC AHB1 peripheral clock enable register
    volatile uint32_t AHB2ENR;    // RCC AHB2 peripheral clock enable register
    volatile uint32_t AHB3ENR;    // RCC AHB3 peripheral clock enable register
    uint32_t RESERVED2;           // Reserved
    volatile uint32_t APB1ENR;    // RCC APB1 peripheral clock enable register
    volatile uint32_t APB2ENR;    // RCC APB2 peripheral clock enable register
    uint32_t RESERVED3[2];        // Reserved
    volatile uint32_t AHB1LPENR;  // RCC AHB1 peripheral clock enable in low power mode register
    volatile uint32_t AHB2LPENR;  // RCC AHB2 peripheral clock enable in low power mode register
    volatile uint32_t AHB3LPENR;  // RCC AHB3 peripheral clock enable in low power mode register
    uint32_t RESERVED4;           // Reserved
    volatile uint32_t APB1LPENR;  // RCC APB1 peripheral clock enable in low power mode register
    volatile uint32_t APB2LPENR;  // RCC APB2 peripheral clock enable in low power mode register
    uint32_t RESERVED5[2];        // Reserved
    volatile uint32_t BDCR;       // RCC Backup domain control register
    volatile uint32_t CSR;        // RCC clock control & status register
    uint32_t RESERVED6[2];        // Reserved
    volatile uint32_t SSCGR;      // RCC Spread spectrum clock generation register
    volatile uint32_t PLLI2SCFGR; // RCC PLLI2S configuration register
} RCC_RegDef_t;

// Register map for TIMER
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RESERVED1;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t RESERVED2;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM_RegDef_t;


// Register map for SysTick
typedef struct {
    volatile uint32_t CTRL;  // SysTick Control and Status Register
    volatile uint32_t LOAD;  // SysTick Reload Value Register
    volatile uint32_t VAL;   // SysTick Current Value Register
    volatile uint32_t CALIB; // SysTick Calibration Value Register
} SysTick_RegDef_t;

// Pointers to GPIO port structures
#define GPIOA ((GPIO_RegDef_t*) GPIOA_BASE)
#define GPIOB ((GPIO_RegDef_t*) GPIOB_BASE)
#define GPIOC ((GPIO_RegDef_t*) GPIOC_BASE)
#define GPIOD ((GPIO_RegDef_t*) GPIOD_BASE)
#define GPIOE ((GPIO_RegDef_t*) GPIOE_BASE)
#define GPIOF ((GPIO_RegDef_t*) GPIOF_BASE)
#define GPIOG ((GPIO_RegDef_t*) GPIOG_BASE)
#define GPIOH ((GPIO_RegDef_t*) GPIOH_BASE)

// Pointers to peripheral registers
#define FLASH    ((FLASH_RegDef_t*) FLASH_BASE)
#define RCC      ((RCC_RegDef_t*) RCC_BASE)
#define SysTick  ((SysTick_RegDef_t*) SYSTICK_BASE)
#define TIM2     ((TIM_RegDef_t*) TIM2_BASE)

// FLASH ACR register bits
#define FLASH_ACR_PRFTEN        (1 << 8) // Prefetch buffer enable
#define FLASH_ACR_LATENCY_5WS   (5 << 0) // Set 5 wait states
#define FLASH_ACR_LATENCY_3WS   (3 << 0) // Set 3 wait states

// RCC PLLCFGR register bits
#define RCC_PLLCFGR_PLLSRC_HSE  (1 << 22) // HSE as PLL source
#define RCC_PLLCFGR_PLLM_Pos    0          // PLLM position
#define RCC_PLLCFGR_PLLN_Pos    6          // PLLN position
#define RCC_PLLCFGR_PLLP_Pos    16         // PLLP position
#define RCC_CFGR_PPRE1_Msk      (0x7 << 10)  // Mask for APB1 prescaler
#define RCC_CFGR_PPRE1_Pos      10           // Position for APB1 prescaler
#define RCC_AHB1ENR_GPIOAEN     (1 << 0) // Enable clock for GPIOA

// Clock control macros
#define GPIOA_CLK_EN()    (RCC->AHB1ENR |= (1 << 0))  // Enable clock for GPIOA
#define GPIOB_CLK_EN()    (RCC->AHB1ENR |= (1 << 1))  // Enable clock for GPIOB
#define GPIOC_CLK_EN()    (RCC->AHB1ENR |= (1 << 2))  // Enable clock for GPIOC
#define GPIOD_CLK_EN()    (RCC->AHB1ENR |= (1 << 3))  // Enable clock for GPIOD


// Clock disable macros
#define GPIOA_CLK_DIS()   (RCC->AHB1ENR &= ~(1 << 0)) // Disable clock for GPIOA
#define GPIOB_CLK_DIS()   (RCC->AHB1ENR &= ~(1 << 1)) // Disable clock for GPIOB
#define GPIOC_CLK_DIS()   (RCC->AHB1ENR &= ~(1 << 2)) // Disable clock for GPIOC
#define GPIOD_CLK_DIS()   (RCC->AHB1ENR &= ~(1 << 3)) // Disable clock for GPIOD
#define RCC_APB1ENR_TIM2EN  ((uint32_t)0x00000001)
#define TIM_CR1_CEN         ((uint16_t)0x0001)

// Timer Base Addresses
#define TIM1_BASE          (APB2PERIPH_BASE + 0x2C00U)
#define TIM2_BASE          (APB1PERIPH_BASE + 0x0000U)
#define TIM3_BASE          (APB1PERIPH_BASE + 0x0400U)
#define TIM4_BASE          (APB1PERIPH_BASE + 0x0800)  // Base address for TIM4
#define TIM_CCMR1_OC1M_Pos      4  // Output Compare Mode position
#define TIM_CCMR1_OC1PE         (1 << 3) // Output Compare Preload Enable
#define TIM_CCER_CC1E           (1 << 0) // Capture/Compare 1 Output Enable

// RCC register bits
#define RCC_CR_HSEON        (1 << 16) // HSE Clock Enable
#define RCC_CR_PLLON       (1 << 24) // PLL Enable
#define RCC_CR_HSERDY      (1 << 17) // HSE Ready
#define RCC_CR_PLLRDY      (1 << 25) // PLL Ready

// RCC_CFGR register bits
#define RCC_CFGR_SW_PLL    (2 << 0) // PLL as system clock
#define RCC_CFGR_SWS      (3 << 2) // System clock switch status
#define RCC_CFGR_SWS_PLL   (2 << 2) // PLL used as system clock
#define RCC_CFGR_SW_CLEAR   (~(uint32_t)(3))  // Clear the lower 2 bits in RCC_CFGR
#define RCC_CFGR_SW_HSE  (1U << 0)  // HSE selected as system clock




#endif /* BASE_H_ */
