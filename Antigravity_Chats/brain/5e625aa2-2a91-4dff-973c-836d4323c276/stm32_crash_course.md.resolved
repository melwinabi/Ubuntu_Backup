# STM32 Bare-Metal Crash Course for Hoags Technologies Interview

This crash course is designed for your specific timeline (4 hours today, 4 hours tomorrow) and tailored to the technical stack of Hoags Technologies (ARM Cortex MCUs, IoT connectivity, peripheral control, and edge AI).

## Level 1: Easy (Fundamentals & Basic I/O) - *Today*
**Focus:** The core of bare-metal programming.
*   **Concepts:** Bare Metal vs. RTOS, MCU Anatomy, Memory Map.
*   **The Golden Rule:** Bitwise Operations (Setting, Clearing, Toggling bits).
*   **Registers:** Reading datasheets to configure hardware.
*   **Practical:** GPIO Configuration (LEDs for output, Buttons/Relays for input).
*   **Interrupts basics:** NVIC (Nested Vectored Interrupt Controller) and ISRs.

## Level 2: Medium (Core Peripherals & Clocks) - *Today*
**Focus:** Making the MCU tick and talk.
*   **Clocks (RCC):** Configuring the internal/external oscillators and PLL (crucial for power and performance).
*   **Timers (TIM):** Creating delays, PWM for motor/buzzer control.
*   **UART/USART:** Serial communication. Essential for debugging, logging, and talking to WiFi/BLE modules.
*   **ADC:** Reading analog sensors (e.g., temperature for home appliances).

## Level 3: Hard (Advanced Peripherals & DMA) - *Tomorrow Morning*
**Focus:** Efficient data handling and sensor communication.
*   **I2C & SPI:** Interfacing with external flash, sensors, and displays.
*   **DMA (Direct Memory Access):** The secret to high-performance IoT. Moving data (like audio or SPI streams) without CPU intervention.
*   **Watchdog Timers (IWDG/WWDG):** Ensuring system reliability in consumer appliances.

## Level 4: Pro (System Level & Interview Specifics) - *Tomorrow Morning*
**Focus:** Bridging bare-metal to Hoags' specific domain (IoT & Voice AI).
*   **Low Power Modes:** Sleep, Stop, Standby (critical for battery/smart devices).
*   **Bootloaders & OTA:** Understanding the firmware update process over the air.
*   **Bare-metal to RTOS:** How your bare-metal drivers fit into FreeRTOS (which Hoags uses).
*   **Audio/AI Basics:** How a microphone (I2S ADC) feeds data to a buffer via DMA, which is then processed by CMSIS-DSP or a TinyML model for voice recognition.

---

## ⚡ Level 1.1: The Absolute Fundamentals

### What is "Bare Metal"?
Bare-metal programming means writing code that interacts **directly with the microcontroller's hardware registers** without an underlying Operating System (like Linux or Windows) or RTOS (FreeRTOS). You are in complete control of the CPU, memory, and timing.

### The Secret Language: Bitwise Operations
In bare metal, you control hardware by changing 1s and 0s in specific memory addresses called **Registers**.
To do this safely without affecting other bits, we use bitwise operators in C: `|` (OR), `&` (AND), `~` (NOT), `^` (XOR), and `<<` / `>>` (Bit Shifts).

#### 1. Setting a bit to 1 (Bitwise OR)
```c
// Set the 5th bit (Bit 5) of the register to 1
REGISTER |= (1 << 5); 
```

#### 2. Clearing a bit to 0 (Bitwise AND with NOT)
```c
// Clear the 5th bit to 0
REGISTER &= ~(1 << 5); 
```

#### 3. Toggling a bit (Bitwise XOR)
```c
// Flip the 5th bit (1 becomes 0, 0 becomes 1)
REGISTER ^= (1 << 5); 
```

#### 4. Checking a bit
```c
// Check if the 5th bit is 1
if (REGISTER & (1 << 5)) {
    // It's 1
}
```

### The Anatomy of an STM32 Project
Every bare-metal system needs these layers to boot up:
1.  **Vector Table:** A list of memory addresses that tell the CPU where to go when the power turns on (Reset_Handler) or when an interrupt happens.
2.  **Startup Code:** Written in assembly or C. It initializes memory, sets up the clock, and then calls your `main()` function.
3.  **Linker Script (.ld file):** Tells the compiler where physical memory (Flash and RAM) is located on the specific STM32 chip.
4.  **main.c:** Your application logic containing an infinite `while(1)` loop. (Embedded devices never "exit" their program).
