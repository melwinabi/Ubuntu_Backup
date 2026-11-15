#include "gpio.h"


void GPIO_Init(GPIO_Handle_t* pGPIOHandle)
{
    // 1. Configure the mode of the GPIO pin
    pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.pinNumber)); // Clear mode bits
    pGPIOHandle->pGPIOx->MODER |= (pGPIOHandle->GPIO_PinConfig.pinMode << (2 * pGPIOHandle->GPIO_PinConfig.pinNumber)); // Set mode bits

    // 2. Configure the output type
    pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.pinNumber); // Clear output type bit
    pGPIOHandle->pGPIOx->OTYPER |= (pGPIOHandle->GPIO_PinConfig.pinOPType << pGPIOHandle->GPIO_PinConfig.pinNumber); // Set output type

    // 3. Configure the speed of the GPIO pin
    pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.pinNumber)); // Clear speed bits
    pGPIOHandle->pGPIOx->OSPEEDR |= (pGPIOHandle->GPIO_PinConfig.pinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.pinNumber)); // Set speed bits

    // 4. Configure the pull-up/pull-down settings
    pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.pinNumber)); // Clear PUPD bits
    pGPIOHandle->pGPIOx->PUPDR |= (pGPIOHandle->GPIO_PinConfig.pinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.pinNumber)); // Set PUPD bits

    // 5. Configure the alternate function (if applicable)
	  if (pGPIOHandle->GPIO_PinConfig.pinMode == GPIO_MODE_AF)
	  {
		  uint8_t afrIdx = pGPIOHandle->GPIO_PinConfig.pinNumber / 8; // Select AFR[0] or AFR[1]
		  uint8_t afrPos = (pGPIOHandle->GPIO_PinConfig.pinNumber % 8) * 4;
		  pGPIOHandle->pGPIOx->AFR[afrIdx] &= ~(0xF << afrPos); // Clear AFR bits OXOF = 11111
		  pGPIOHandle->pGPIOx->AFR[afrIdx] |= (pGPIOHandle->GPIO_PinConfig.pinAltFunMode << afrPos); // Set AFR bits
	  }
}


// Function to write to a GPIO output pin
void GPIO_WriteToOutputPin(GPIO_RegDef_t* pGPIOx, uint8_t PinNumber, uint8_t Value) {
    if (Value == 1) {
        pGPIOx->BSRR = (1 << PinNumber); // Set the pin
    } else {
        pGPIOx->BSRR = (1 << (PinNumber + 16)); // Reset the pin
    }
}

// Function to read from a GPIO input pin
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t* pGPIOx, uint8_t PinNumber) {
    return (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x1); // Return the state of the pin
}

// Function to toggle a GPIO output pin
void GPIO_ToggleOutputPin(GPIO_RegDef_t* pGPIOx, uint8_t PinNumber) {
    pGPIOx->ODR ^= (1 << PinNumber); // Toggle the pin
}
