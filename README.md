# Quad_PNI_RM3100_STM32F103C8_MT25QL01GBBB_Flash
## Overview

This project is designed to interface the Quad-PNI RM3100 magnetic sensor with an STM32F103C8 microcontroller and write in flash MT25QL01GBBB. The RM3100 provides highly accurate magnetic field measurements, and this project demonstrates how to read, process and write these measurements data in MT25QL01GBBB flash using an STM32 microcontroller.

## Features

- **Magnetic Field Sensing**: Read magnetic field data from the Quad-RM3100 sensor(Four RM3100).
- **Data Processing**: Process the raw data from the sensor to obtain usable magnetic field values.
- **Communication**: Uses SPI communication protocols to interface with the sensor and Flash memory.
- **Microcontroller**: Based on the STM32F103C8, a low-cost, high-performance ARM Cortex-M3 microcontroller.

## Getting Started

### Prerequisites

- **Hardware**:
  - STM32F103C8 microcontroller
  - PNI RM3100 magnetic sensor
  - Development board (e.g., Blue Pill)
  - Connecting wires or PCB for connections
  - MT25QL01GBBB Flash memory
- **Software**:
  - STM32CubeIDE or any compatible IDE
  - STM32CubeMX for configuration
  - ARM GCC Toolchain

### Installation

1. **Clone the Repository**:
    ```sh
    git clone https://github.com/your_username/Quad_PNI_RM3100_STM32F103C8_MT25QL01GBBB_Flash.git
    cd Quad_PNI_RM3100_STM32F103C8_MT25QL01GBBB_Flash
    ```

2. **Open the Project**:
    - Open STM32CubeIDE and import the project.
    - Alternatively, use STM32CubeMX to generate the project files and then open them in your preferred IDE.

3. **Configure the Microcontroller**:
    - Set up the SPI interface in the STM32CubeMX or your IDE.
    - Configure the GPIO pins according to your hardware setup.

4. ** Flash Memory**:
    - Set up SPI interface in MT25QL01GBBB Flash memory.
    - Configure the GPIO pins according to your hardware setup.

### Usage

1. **Power up the system**.
2. **Run the firmware**.
3. **Observe the output**:
    - Use a serial monitor to read the magnetic field data output.
    - Connect to the microcontroller via UART to observe data in real-time.

### Documentation

- [RM3100 Datasheet](https://www.pnicorp.com/download/12261/)
- [STM32F103C8 Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [STM32CubeIDE User Guide](https://www.st.com/resource/en/user_manual/dm00373530.pdf)
