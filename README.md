# PrintEaseFirmware

PrintEaseFirmware is the embedded software for the PrintEase automated printing system. Written in C++ using the Arduino framework, this firmware runs on kiosk devices, allowing users to select print options and initiate jobs on networked printers managed by the PrintEaseApp backend.

## Key Features

- **User Interaction via Kiosk**
  - Users can select paper size (A4, A3, A5, Letter, Legal).
  - Choose print type (Simplex/Duplex).
  - Select color mode (Black & White or Color).
  - Multiple options supported for flexible printing needs.

- **Network Communication**
  - Communicates with backend server via REST APIs and WebSocket for real-time job handling.
  - Sends print job requests and receives status updates.

- **Printer Queue Management**
  - Handles job queuing and error reporting.
  - Provides feedback to users on job status via kiosk interface.

- **Configuration**
  - Easy setup for Wi-Fi/network credentials and backend endpoints.

## Architecture Overview

- **Kiosk Device (ESP32/Arduino):** Runs this firmware, provides touchscreen/user interface, and connects to network.
- **Backend Server:** Manages printers, pricing, and statistics using Django-based PrintEaseApp.
- **Communication:** REST APIs for job submission, WebSocket for live updates.

## Technologies Used

- **Language:** C++ (Arduino framework)
- **Platform:** ESP32 or compatible Arduino device
- **Network:** Wi-Fi, HTTP/REST, WebSocket

## Setup & Installation

1. **Clone the repository:**
    ```sh
    git clone https://github.com/RN1818/PrintEaseFirmware.git
    cd PrintEaseFirmware
    ```

2. **Install Arduino IDE and required libraries:**
    - ESP32 board support
    - WiFi, HTTPClient, WebSockets, Touchscreen libraries (see code for specifics)

3. **Configure firmware:**
    - Set Wi-Fi credentials and backend server URL in `config.h` or setup section.

4. **Upload to device:**
    - Connect your ESP32/Arduino device to your computer.
    - Use Arduino IDE to compile and upload the firmware.

## Usage

- Power on the kiosk device.
- Use the touchscreen or physical interface to select print options.
- Firmware will communicate with the backend and display job status.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

For more information, visit the [PrintEase homepage](https://print-ease-app.vercel.app) or refer to the backend repository [PrintEaseApp](https://github.com/RN1818/PrintEaseApp).