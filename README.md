# ⚡ STM32 Bare-Metal Sensor System (I2C, SPI, UART)

> High-performance, register-level bare-metal firmware developed for the **STM32F401CCU6** (Black Pill) microcontroller. Interfaces with a **BMP280 Barometric Pressure & Temperature Sensor** (via SPI) and an **LM75 Temperature Sensor** (via I2C), transmitting real-time compensated values over **USART1**.

<p align="center">
  <img src="https://img.shields.io/badge/Platform-STM32F401--BlackPill-blue?style=for-the-badge&logo=stmicroelectronics" alt="Platform"/>
  <img src="https://img.shields.io/badge/Language-C%20%2F%20Assembly-orange?style=for-the-badge" alt="Language"/>
  <img src="https://img.shields.io/badge/Style-Register--Level-green?style=for-the-badge" alt="Style"/>
</p>

---

## 🚀 Project Overview

This repository demonstrates bare-metal firmware development using direct memory-mapped register pointer dereferencing. Designed entirely without dependency on ST HAL or LL libraries, this project showcases low-level microcontroller initialization, bus timing configurations, and sensor protocol implementations.

### 🌟 Key Technical Highlights
* **Direct Register Manipulation** — Utilizes raw memory-mapped pointer casting (e.g. `*(volatile uint32_t*)`) to configure GPIO registers, peripheral clocks, and communication controllers.
* **Robust Hardware I2C Driver & LM75 Integration**:
  * Configures hardware I2C1 Master Mode at **100 kHz** standard speed.
  * Implements strict bus signaling sequences (START, Repeated START, ACK/NACK, and STOP conditions) compliant with the STM32 Reference Manual.
  * Incorporates **software timeout safety loops** to prevent CPU lock-ups in case of communication or sensor disconnects.
  * Handles signed 11-bit I2C temperature readings (0.5°C step resolution).
* **Hardware SPI Driver & BMP280 Integration**:
  * Configures SPI1 Master Mode (Mode 0) using software-driven chip select (`PA4`).
  * Retrieves device-specific factory-calibrated parameters from internal sensor ROM.
  * Implements **Bosch calibration & compensation math** using 64-bit integer calculations for high-precision physical units.
* **USART1 Debug Logger**:
  * Configured at **9600 baud** using a 16 MHz internal HSI clock.
  * Safe float-to-string conversion separating integer and fractional parts, eliminating the dependency on heavy standard float-printf libraries.

---

## 🏗️ System Architecture & Connection

```
                   ┌──────────────────────────────────┐
                   │        STM32F401CCU6 (MCU)       │
                   │                                  │
                   │   USART1    I2C1       SPI1      │
                   │  (PA9/PA10) (PB6/PB7) (PA4..PA7) │
                   └──────┬──────────┬──────────┬─────┘
                          │          │          │
                          ▼          ▼          ▼
                    ┌──────────┐┌──────────┐┌──────────┐
                    │  USB-TTL ││   LM75   ││  BMP280  │
                    │  Logger  ││ Temp     ││ Pressure │
                    │  9600 Bd ││ Sensor   ││ Sensor   │
                    └──────────┘└──────────┘└──────────┘
```

### Pin Mappings
* **BMP280 Sensor (SPI1)**:
  * `PA4` ──▶ **CS** (Software Chip Select)
  * `PA5` ──▶ **SCK** (Serial Clock)
  * `PA6` ──▶ **MISO** (Master In Slave Out)
  * `PA7` ──▶ **MOSI** (Master Out Slave In)
* **LM75 Sensor (I2C1)**:
  * `PB6` ──▶ **SCL** (Serial Clock, AF4, Open-Drain)
  * `PB7` ──▶ **SDA** (Serial Data, AF4, Open-Drain)
* **USB-to-UART Converter (USART1)**:
  * `PA9` ──▶ **TX** (Transmit, AF7)
  * `PA10` ──▶ **RX** (Receive, AF7)

---

## 📦 File Structure

```
STM32-I2C-SPI-UART-baremetal/
│
├── Src/
│   ├── lib/
│   │   ├── GPIO.h                # RCC & GPIO register definitions
│   │   ├── i2c.h / i2c.c         # Hardware I2C driver & LM75 monitoring
│   │   ├── spi.h / spi.c         # Hardware SPI driver & BMP280 calibration math
│   │   └── uart.h / uart.c       # Hardware USART1 driver (9600 baud)
│   │
│   ├── main.c                   # Main system monitoring loop
│   ├── syscalls.c               # Standard system calls
│   └── sysmem.c                 # Memory management functions
│
├── Startup/
│   └── startup_stm32f401ccux.s  # Vector table & reset handler in Assembly
│
├── STM32F401CCUX_FLASH.ld       # Linker script defining flash/RAM layout
├── .gitignore                   # Ignore build artifacts (Debug/)
├── INTERVIEW_PREP.md            # 📚 Study guide with core bare-metal concepts
└── README.md                    # Project documentation
```

---

## ⚙️ How to Build & Run

1. **Prerequisites**: Install **STM32CubeIDE** (or GCC ARM Toolchain + Make).
2. **Importing Project**:
   * Open STM32CubeIDE.
   * Go to **File > Import... > General > Existing Projects into Workspace**.
   * Browse to the repository root directory and import it.
3. **Build & Flash**:
   * Connect an **ST-Link V2** programmer to the target Black Pill board.
   * Press **Build** (Hammer icon) and then **Debug** or **Run** (Play icon).
4. **Read Output**:
   * Connect a USB-to-UART module to PA9 (TX) and GND.
   * Open your favorite serial monitor (PuTTY, TeraTerm) at **9600 baud**.
   * Monitor output in real-time:
     ```text
     BMP280: 25.14 C, 101325.04 Pa | LM75: 25.50 C
     BMP280: 25.15 C, 101324.98 Pa | LM75: 25.50 C
     ```

---

## 💡 Engineering Implementation Details

### Low-Level Register Mapping
Hardware registers are defined using memory-mapped pointers with the C `volatile` keyword to tell the compiler that these memory slots can be modified by the hardware asynchronously:
```c
#define SPI1_BASE       0x40013000
#define SPI1_CR1        (*((volatile uint32_t*)(SPI1_BASE + 0x00)))
#define SPI1_SR         (*((volatile uint32_t*)(SPI1_BASE + 0x08)))
#define SPI1_DR         (*((volatile uint32_t*)(SPI1_BASE + 0x0C)))
```

### Timeout Safety Loops
To ensure the system remains responsive even if communication fails, all block-waiting loops include a simple decrementing timeout counter:
```c
uint32_t timeout = 50000;
while (!(I2C1_SR1 & (1 << 0)) && --timeout); // Wait for SB
if (timeout == 0) return -999.0f;            // Safely exit rather than lock up
```
This is a standard industrial safety pattern for mission-critical embedded software.

---

## 📬 Contact
* **Email**: [huyphan1455@gmail.com](mailto:huyphan1455@gmail.com)
* **LinkedIn**: [phankhachuy](https://www.linkedin.com/in/phankhachuy/)
* **GitHub**: [@PKHuy1455](https://github.com/PKHuy1455)
