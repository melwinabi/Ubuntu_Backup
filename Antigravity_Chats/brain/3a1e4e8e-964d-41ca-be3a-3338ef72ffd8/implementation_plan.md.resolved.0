# STM32F407 Bare-Metal LED Blink Project

This project implements a simple LED blink program for the STM32F407VGT6 microcontroller using bare-metal programming techniques. The code will directly manipulate registers to control a GPIO pin connected to an LED.

## Proposed Changes

### Configuration Files

#### [NEW] [base.h](file:///home/melwinabi/.gemini/antigravity/scratch/stm32f407_blink/base.h)
Defines the base addresses for the RCC and GPIOD peripherals.

#### [NEW] [gpio.h](file:///home/melwinabi/.gemini/antigravity/scratch/stm32f407_blink/gpio.h)
Defines the register offsets and bit macros for GPIO configuration and control.

### Main Application

#### [NEW] [main.c](file:///home/melwinabi/.gemini/antigravity/scratch/stm32f407_blink/main.c)
Contains the main loop that initializes the GPIO and toggles the LED with a software delay.

## Verification Plan

### Automated Tests
- None (Hardware-dependent bare-metal code is difficult to test automatically without a simulator).

### Manual Verification
1. Review the register addresses against the STM32F407 reference manual (RM0090).
2. Verify bit masks and shifts for RCC and GPIO registers.
3. Check the software delay implementation for basic functionality.
4. User can compile the code using `arm-none-eabi-gcc` and flash it to the board to verify the LED blink.
