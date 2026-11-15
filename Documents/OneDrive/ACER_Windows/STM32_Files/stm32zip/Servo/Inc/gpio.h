/*
 * gpio.h
 *
 *  Created on: Sep 26, 2024
 *      Author: Administrator
 */

#ifndef GPIO_H
#define GPIO_H

#include "base.h"
// GPIO Mode Macros
#define GPIO_MODE_INPUT      0x0
#define GPIO_MODE_OUTPUT     0x1
#define GPIO_MODE_AF         0x2
#define GPIO_MODE_ANALOG     0x3

// GPIO Output Type Macros
#define GPIO_OP_TYPE_PP      0x0
#define GPIO_OP_TYPE_OD      0x1

// GPIO Speed Macros
#define GPIO_SPEED_LOW       0x0
#define GPIO_SPEED_MEDIUM    0x1
#define GPIO_SPEED_HIGH      0x2
#define GPIO_SPEED_VERY_HIGH 0x3

// GPIO Pull-Up/Pull-Down Macros
#define GPIO_NO_PUPD         0x0
#define GPIO_PU              0x1
#define GPIO_PD              0x2

// GPIO Pin Configuration Structure
typedef struct {
    uint8_t pinNumber;        // Pin number
    uint8_t pinMode;          // Mode of the pin
    uint8_t pinOPType;        // Output type
    uint8_t pinSpeed;         // Speed of the pin
    uint8_t pinAltFunMode;
    uint8_t pinPuPdControl;   // Pull-up/Pull-down configuration
} GPIO_PinConfig_t;

// GPIO Handle Structure
typedef struct {
    GPIO_RegDef_t* pGPIOx;     // Pointer to GPIO peripheral
    GPIO_PinConfig_t GPIO_PinConfig; // Pin configuration
} GPIO_Handle_t;

// Base addresses for GPIO ports

// GPIO register structure definition


// Function prototypes
void GPIO_Init(GPIO_Handle_t* pGPIOHandle);
void GPIO_WriteToOutputPin(GPIO_RegDef_t* pGPIOx, uint8_t PinNumber, uint8_t Value); //BSRR
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t* pGPIOx, uint8_t PinNumber);//IDR
void GPIO_ToggleOutputPin(GPIO_RegDef_t* pGPIOx, uint8_t PinNumber);//ODR

#endif // GPIO_H
