# STM32F407 Bare-Metal LED Blink Code Walkthrough

I have implemented the bare-metal code for STM32F407VGT6 to blink an LED on PD12 (typically the Green LED on the Discovery board).

## Changes Made

### Configuration Files
- [base.h](file:///home/melwinabi/.gemini/antigravity/scratch/stm32f407_blink/base.h): Contains the essential register base addresses for RCC and GPIOD.
- [gpio.h](file:///home/melwinabi/.gemini/antigravity/scratch/stm32f407_blink/gpio.h): Defines the bit-level macros for enabling the peripheral clock and configuring the GPIO pin mode.

### Main Application
- [main.c](file:///home/melwinabi/.gemini/antigravity/scratch/stm32f407_blink/main.c): Implements the blink logic with a simple software delay loop.

## How to Compile and Flash

To compile this code, you will need the `arm-none-eabi` toolchain. Note that for a complete bare-metal project, you also need a **startup file** and a **linker script** specific to the STM32F407, which handles vector table placement and memory layout.

### Sample Compilation Command
```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -c main.c -o main.o
```

## Verification Results

### Manual Code Review
- **RCC Base**: `0x40023800` (Matches RM0090).
- **AHB1ENR**: `RCC_BASE + 0x30` enables bit 3 for GPIOD (Correct).
- **GPIOD Base**: `0x40020C00` (Matches RM0090).
- **PD12 Configuration**:
  - `MODER` bits 25:24 set to `01` for Output (Correct).
  - `ODR` bit 12 toggles for PD12 (Correct).
- **Software Delay**: Uses `volatile` to prevent compiler optimization (Correct).
