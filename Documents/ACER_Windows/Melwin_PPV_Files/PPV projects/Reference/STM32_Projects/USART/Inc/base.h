#ifndef BASE_H_
#define BASE_H_

#include <stdint.h>

#define _VO volatile

#define PERIPHERAL_BASE          (0x40000000)

#define APB1_PERIPHERAL_OFFSET   (0x00000000)
#define APB1_PERIPHERAL          (PERIPHERAL_BASE + APB1_PERIPHERAL_OFFSET)

#define APB2_PERIPHERAL_OFFSET   (0x00010000)
#define APB2_PERIPHERAL          (PERIPHERAL_BASE + APB2_PERIPHERAL_OFFSET)

#define AHB1_PERIPHERAL_OFFSET   (0x00020000)
#define AHB1_PERIPHERAL          (PERIPHERAL_BASE + AHB1_PERIPHERAL_OFFSET)

#define GPIOA_PERIPHERAL_OFFSET  (0x00000000)
#define GPIOA_PERIPHERAL         (AHB1_PERIPHERAL + GPIOA_PERIPHERAL_OFFSET)

#define GPIOB_PERIPHERAL_OFFSET  (0x00000400)
#define GPIOB_PERIPHERAL         (AHB1_PERIPHERAL + GPIOB_PERIPHERAL_OFFSET)

#define GPIOC_PERIPHERAL_OFFSET  (0x00000800)
#define GPIOC_PERIPHERAL         (AHB1_PERIPHERAL + GPIOC_PERIPHERAL_OFFSET)

#define GPIOH_PERIPHERAL_OFFSET  (0x00001C00)
#define GPIOH_PERIPHERAL         (AHB1_PERIPHERAL + GPIOH_PERIPHERAL_OFFSET)

#define RCC_PERIPHERAL_OFFSET    (0x00023800)
#define RCC__PERIPHERAL          (PERIPHERAL_BASE + RCC_PERIPHERAL_OFFSET)

#define USART1_PERIPHERAL_OFFSET (0x00001000)
#define USART1_PERIPHERAL        (APB2_PERIPHERAL + USART1_PERIPHERAL_OFFSET)

#define USART2_PERIPHERAL_OFFSET (0x00004400)
#define USART2_PERIPHERAL        (APB1_PERIPHERAL + USART2_PERIPHERAL_OFFSET)

#define TIM6_PERIPHERAL_OFFSET   (0x00001000)
#define TIM6_PERIPHERAL          (APB1_PERIPHERAL + TIM6_PERIPHERAL_OFFSET)

typedef struct {
  volatile uint32_t MODER;
  volatile uint32_t OTYPER;
  volatile uint32_t OSPEEDR;
  volatile uint32_t PUPDR;
  volatile uint32_t IDR;
  volatile uint32_t ODR;
  volatile uint32_t BSRR;
  volatile uint32_t LCKR;
  volatile uint32_t AFRL;
  volatile uint32_t AFRH;
} GPIO_TypeDef;

typedef struct {
  volatile uint32_t CR;
  volatile uint32_t PLLCFGR;
  volatile uint32_t CFGR;
  volatile uint32_t CIR;
  volatile uint32_t AHB1RSTR;
  volatile uint32_t AHB2RSTR;
  uint32_t RESERVED0[2];
  volatile uint32_t APB1RSTR;
  volatile uint32_t APB2RSTR;
  uint32_t RESERVED1[2];
  volatile uint32_t AHB1ENR;
  volatile uint32_t AHB2ENR;
  uint32_t RESERVED2[2];
  volatile uint32_t APB1ENR;
  volatile uint32_t APB2ENR;
  uint32_t RESERVED3[2];
  volatile uint32_t AHB1LPENR;
  volatile uint32_t AHB2LPENR;
  uint32_t RESERVED4[2];
  volatile uint32_t APB1LPENR;
  volatile uint32_t APB2LPENR;
  uint32_t RESERVED5[2];
  volatile uint32_t BDCR;
  volatile uint32_t CSR;
  uint32_t RESERVED6[2];
  volatile uint32_t SSCGR;
  volatile uint32_t PLLI2SCFGR;
} RCC_TypeDef;

typedef struct {
  volatile uint32_t SR;
  volatile uint32_t DR;
  volatile uint32_t BRR;
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t CR3;
  volatile uint32_t GTPR;
} USART_TypeDef;

typedef struct {
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t RESERVED0;
  volatile uint32_t DIER;
  volatile uint32_t SR;
  volatile uint32_t EGR;
  uint32_t RESERVED1[3];
  volatile uint32_t CNT;
  volatile uint32_t PSC;
  volatile uint32_t ARR;
} TIM6_TypeDef;

#endif /* BASE_H_ */
