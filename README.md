# Digital Oscilloscope using STM32

A portable **digital oscilloscope** built around the STM32F091RC microcontroller and ILI9341 TFT display, developed as a final project for ECEN 5613 at the University of Colorado Boulder. It offers real-time waveform display, voltage and frequency measurement, and interactive settings adjustment—ideal for embedded systems learning and prototyping[attached_file:1].

---

## Overview

This oscilloscope project solves the problem of needing a bulky benchtop instrument by enabling signal monitoring anywhere. It provides core oscilloscope functions such as waveform capture, min/max/peak-to-peak voltage measurement, frequency calculation, and a full on-screen UI, all in a compact and affordable format[attached_file:1].

---

## Features

- **Real-time signal acquisition** with 12-bit ADC, up to 1 MSPS[attached_file:1]
- **DMA-based buffering** for efficient sampling without CPU load[attached_file:1]
- **240x320 color TFT SPI display** for waveform and UI[attached_file:1]
- **Voltage, frequency, and peak-to-peak voltage measurements** on-screen[attached_file:1]
- **User-adjustable parameters** via push buttons:
  - V/div (vertical sensitivity)
  - Time/div (horizontal timebase)
  - Trigger Level
  - Attenuation (e.g., 1x, 10x)[attached_file:1]
- **Grid and trace rendering** for oscilloscope-style visualization[attached_file:1]
- **Intuitive settings menu** for easy navigation and adjustment[attached_file:1]

---

## Hardware

- **STM32F091RC microcontroller**
  - 12-bit ADC, DMA, SPI, timers, I2C/UART[attached_file:1]
- **ILI9341 TFT Display** (2.8", SPI, 240x320, 262K colors)[attached_file:1]
- **Push buttons** for settings navigation and changes[attached_file:1]
- **Prototype board and connectors**[attached_file:1]

### Key Pin Connections

| Pin Name | Function                  | STM32F091RC Pin | Display/Other   |
|----------|---------------------------|-----------------|-----------------|
| PB3      | SPI Clock (SCK)           | SCK             | ILI9341         |
| PA7      | SPI MOSI                  | MOSI            | ILI9341         |
| PA6      | SPI MISO (unused)         | (Unused)        |                 |
| PA0      | ADC Input Channel         | ADC_IN0         | Signal Input    |
| PB0      | Chip Select (CS)          | GPIO            | ILI9341         |
| PB1      | Data/Command Control (DC) | GPIO            | ILI9341         |
| PB2      | Display Reset (RST)       | GPIO            | ILI9341         |
| PB6/B7   | I2C SCL/SDA (debug)       | GPIO            | (Reserved)      |
| PB12     | Ground (GND)              | GND             | Common GND      |

---

## Firmware and Code Design

- Built with **STM32CubeIDE** using **HAL libraries** for abstraction[attached_file:1]
- Core firmware functions:
  - ADC+DMA sampling and buffering with timer-based acquisition
  - Conversion of ADC values to voltage, min/max/Vpp calculations
  - Trigger point and frequency determination from signal buffer
  - Flexible pixel-to-voltage/timebase scaling formulas
  - Grid, trace, and parameter rendering functions for the display
  - Button interrupt handlers for real-time settings adjustment[attached_file:1]
- UI includes grid drawing, live waveform trace, and info bar showing measured values[attached_file:1]
- Settings menu for V/div, Time/div, Trigger, Attenuation—navigated and modified using 3 hardware buttons[attached_file:1]

---

## Bill of Materials

| Part                                  | Source                 | Cost   |
|--------------------------------------- |------------------------|--------|
| ILI9341 2.8” SPI TFT LCD Display      | Amazon                 | $16.39 |
| NUCLEO-F091RC Dev Board               | DigiKey                | $11.04 |
| Prototype Board                       | ECEN E-store           | $7.79  |
| SparkFun Button                       | SparkFun Electronics   | $1.75  |
| **Total**                             |                        | $37.97 |

---

## Getting Started

### Hardware

1. Connect display wires per the pinout above[attached_file:1].
2. Connect push buttons to GPIOs (using pull-ups and interrupts)[attached_file:1].
3. Make common ground across all connected modules[attached_file:1].
4. Use a signal/function generator as the signal input for best results[attached_file:1].

### Firmware

1. Load and open the provided firmware code (see appendices for code repository if available)[attached_file:1].
2. Build in STM32CubeIDE for the STM32F091RC target[attached_file:1].
3. Flash onto the microcontroller board[attached_file:1].

---

## Usage

- Navigate the settings menu using the hardware buttons.
- Adjust V/div, Time/div, trigger level, and attenuation on the fly.
- View real-time waveform traces and numeric signal measurements.
- Compare readings with standard oscilloscopes for verification[attached_file:1].

---

## Testing and Known Limitations

- Tested with sine, square, and triangle waves from a function generator[attached_file:1].
- Verified voltage and frequency on a commercial oscilloscope[attached_file:1].
- **Limitation:** Display refresh rate is slower than desired for high-speed signals; future improvements may include using a higher-sample-rate ADC[attached_file:1].

---

## Author

Jainil Patel
