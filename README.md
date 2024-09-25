# Arduino Serial Microservices

![Arduino](https://img.shields.io/badge/Arduino-Microservices-blue)
![EEPROM](https://img.shields.io/badge/EEPROM-Storage-green)
![Protocol](https://img.shields.io/badge/Protocol-Shared-yellow)
![Hashing](https://img.shields.io/badge/Hashing-Secure-orange)

## Overview

`arduino-serial-microservices` is a project that demonstrates the use of microservices architecture with Arduino. This project leverages EEPROM for persistent storage, a shared protocol for communication between services, and hashing for secure data transmission.

## Features

- **Microservices Architecture**: Modular design with independent services.
- **EEPROM Storage**: Persistent storage using EEPROM.
- **Shared Protocol**: Common protocol for communication between microservices.
- **Hashing**: Secure data transmission using hashing algorithms.

## Components

1. **Arduino Microcontroller**: The core hardware component.
2. **EEPROM**: Used for storing persistent data.
3. **Microservices**: Independent services running on the Arduino.
4. **Shared Protocol**: Protocol for communication between microservices.
5. **Hashing**: Ensures data integrity and security.

## Getting Started

### Prerequisites

- Arduino IDE
- Arduino board with EEPROM support
- Required libraries:
  - `EEPROM.h`
  - `Hash.h`

### Installation

1. **Clone the repository**:
    ```sh
    git clone https://github.com/caiotheodoro/arduino-serial-microservices.git
    cd arduino-serial-microservices
    ```

2. **Open the project in Arduino IDE**:
    - Open Arduino IDE.
    - Navigate to `File > Open` and select the `arduino-serial-microservices.ino` file.

3. **Install required libraries**:
    - Go to `Sketch > Include Library > Manage Libraries`.
    - Search for `EEPROM` and `Hash` libraries and install them.

### Usage

1. **Upload the code**:
    - Connect your Arduino board to your computer.
    - Select the correct board and port from `Tools > Board` and `Tools > Port`.
    - Click on the upload button to upload the code to the Arduino.

2. **Run the microservices**:
    - Once the code is uploaded, the microservices will start running on the Arduino.
    - The services will communicate using the shared protocol and store data in EEPROM.

