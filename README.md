# ⚡ STM32 Bare-Metal Sensor Monitor (I2C, SPI, UART)

> A high-performance, register-level bare-metal firmware for **STM32F401CCU6** (Black Pill) that interfaces with a **BMP280** pressure/temperature sensor (via SPI) and an **LM75** temperature sensor (via I2C), transmitting real-time readings over UART. Built completely from scratch without ST HAL/LL libraries.

<p align="center">
  <img src="https://img.shields.io/badge/Platform-STM32F401--BlackPill-blue?style=for-the-badge&logo=stmicroelectronics" alt="Platform"/>
  <img src="https://img.shields.io/badge/Language-C%20%2F%20Assembly-orange?style=for-the-badge" alt="Language"/>
  <img src="https://img.shields.io/badge/Standard-CMSIS%20Struct%20Mapping-green?style=for-the-badge" alt="Standard"/>
</p>

---

## 🚀 Key Features

* **Zero HAL/LL Overhead** — Directly manipulates hardware registers for maximum execution speed and minimal flash footprint.
* **Professional CMSIS-Style Abstraction** — Implements a custom `stm32f401.h` header with clean `struct` memory-mappings for RCC, GPIO, USART, I2C, and SPI peripherals.
* **SPI Driver & BMP280 Integration**:
  * Configured in Master Mode (Mode 0) at ~1 MHz.
  * Implements high-speed **6-byte SPI burst-reads** to retrieve pressure and temperature data in a single transaction.
  * Full implementation of **Bosch calibration & compensation math** (32-bit/64-bit integer algorithms) for highly accurate physical values.
* **I2C Driver & LM75 Integration**:
  * Hardware I2C Master Mode operating at 100 kHz.
  * Multi-byte read transfer sequence with proper START, Repeated START, ACK/NACK control, and STOP generation.
  * 11-bit signed temperature calculation (0.125°C resolution).
* **UART Logger**:
  * Configured at 115200 baud with HSI 16 MHz clock.
  * Custom safe float-to-string formatting (prints float values as integer + fractional parts to avoid heavy printf float library linking overhead).

---

## 🏗️ Hardware Architecture & Connections

### Pin Mappings
* **BMP280 Sensor (SPI1)**:
  * `PA4` ──▶ **CS** (Software Chip Select)
  * `PA5` ──▶ **SCK** (Serial Clock)
  * `PA6` ──▶ **MISO** (Master In Slave Out)
  * `PA7` ──▶ **MOSI** (Master Out Slave In)
* **LM75 Sensor (I2C1)**:
  * `PB6` ──▶ **SCL** (Serial Clock, AF4, Open-Drain)
  * `PB7` ──▶ **SDA** (Serial Data, AF4, Open-Drain)
* **USB-to-UART Logger (USART1)**:
  * `PA9` ──▶ **TX** (Transmit, AF7)
  * `PA10` ──▶ **RX** (Receive, AF7)

---

## 📦 Project Structure

```
STM32-I2C-SPI-UART-baremetal/
│
├── Final/
│   ├── Src/
│   │   ├── lib/
│   │   │   ├── stm32f401.h          # ⚡ CMSIS-style struct register definitions
│   │   │   ├── GPIO.h                #    GPIO configuration macros
│   │   │   ├── i2c.h / i2c.c         #    Hardware I2C driver + LM75 sensor functions
│   │   │   ├── spi.h / spi.c         #    Hardware SPI driver + BMP280 sensor & math
│   │   │   └── uart.h / uart.c       #    Hardware USART1 driver (115200 baud)
│   │   │
│   │   ├── main.c                   #    Main monitoring loop (burst reads + print)
│   │   ├── syscalls.c
│   │   └── sysmem.c
│   │
│   ├── Startup/
│   │   └── startup_stm32f401ccux.s  #    Reset handler and vector table
│   │
│   ├── STM32F401CCUX_FLASH.ld       #    Linker script (defines memory layout)
│   └── Final Debug.launch           #    Debug launcher configuration
│
├── .gitignore                       #    Ignores build/Debug outputs
└── README.md
```

---

## ⚙️ How to Build & Run

### 1. Requirements
* **STM32CubeIDE** (or GCC ARM Embedded Toolchain + Make)
* **Black Pill F401CCU6** board
* **ST-Link V2** debugger/programmer
* USB-to-UART converter (PL2303, CP2102, or similar) connected to PA9/PA10

### 2. Importing into STM32CubeIDE
1. Open STM32CubeIDE.
2. Select **File > Import...**
3. Choose **General > Existing Projects into Workspace**.
4. Select the `Final/` directory as the root directory.
5. Click **Finish**.

### 3. Running & Monitoring
* Connect ST-Link V2 to the Black Pill board.
* Connect USB-to-UART converter to PA9 (TX) and GND, then plug it into your computer.
* Open a serial terminal (PuTTY, TeraTerm, or Arduino Serial Monitor) at **115200 baud**.
* Build and flash the project from the IDE. You will see output like this:

```text
=== STM32 Baremetal Sensor Monitor Started ===
BMP280: 25.14 C, 1013.25 hPa | LM75: 25.50 C
BMP280: 25.15 C, 1013.24 hPa | LM75: 25.50 C
BMP280: 25.17 C, 1013.26 hPa | LM75: 25.62 C
```

---

## 💡 Key Engineering Details

### ⚡ Struct Memory Mapping Abstraction
Using C structures to map hardware registers allows compiler-level offset checks, preventing manual register offset calculation mistakes:
```c
typedef struct {
    volatile uint32_t MODER;         /* GPIO port mode register,          Address offset: 0x00 */
    volatile uint32_t OTYPER;        /* GPIO port output type register,   Address offset: 0x04 */
    volatile uint32_t OSPEEDR;       /* GPIO port output speed register,  Address offset: 0x08 */
    volatile uint32_t PUPDR;         /* GPIO port pull-up/pull-down reg,  Address offset: 0x0C */
    volatile uint32_t IDR;           /* GPIO port input data register,    Address offset: 0x10 */
    volatile uint32_t ODR;           /* GPIO port output data register,   Address offset: 0x14 */
    volatile uint32_t BSRR;          /* GPIO port bit set/reset register, Address offset: 0x18 */
    volatile uint32_t LCKR;          /* GPIO port configuration lock reg, Address offset: 0x1C */
    volatile uint32_t AFR[2];        /* GPIO alternate function registers,Address offset: 0x20-0x24 */
} GPIO_TypeDef;
```

### 🧠 High-Efficiency Burst Readings
Rather than reading registers one by one (requiring toggling the SPI Chip Select pin 6 times), the firmware toggles CS once and does a continuous read of 6 registers (`0xF7` through `0xFC`) to retrieve raw pressure and temperature simultaneously. This saves CPU clock cycles and lowers bus utilization.

---

## 📬 Contact
* **Email**: [huyphan1455@gmail.com](mailto:huyphan1455@gmail.com)
* **LinkedIn**: [linkedin/phankhachuy](https://www.linkedin.com/in/phankhachuy/)
* **GitHub**: [@PKHuy1455](https://github.com/PKHuy1455)
