## 📖 Description
This project demonstrates SPI (Serial Peripheral Interface) communication between an STM32F401 (Master) and an Arduino UNO (Slave).

**The STM32 sends commands to the Arduino to:**
- Control an LED
- Read analog sensor data
- Read LED status
- Print messages
- Read device ID
---

## 🚀 Features
- Full-duplex SPI communication
- Custom command-based protocol
- Master: STM32F401
- Slave: Arduino UNO
- LED control and status monitoring
- Analog sensor data acquisition
- Message transmission over SPI
- Device ID retrieval

---

## 🧠 Project Structure
```
├── main.c
├── STM32F401_GPIO.c
├── STM32F401_GPIO.h
├── STM32F401_SPI.c
├── STM32F401_SPI.h
├── STM32F401XX.h

```

---

## 🔧 Hardware Connections

| STM32F401 Pin | Function   | Arduino UNO Pin |
|--------------|-----------|-----------------|
| PB13         | SCLK      | D13 (SCK)       |
| PB15         | MOSI      | D11 (MOSI)      |
| PB14         | MISO      | D12 (MISO)      |
| PB12         | NSS       | D10 (SS)        |
| PA0          | Button    | —               |
| GND          | Ground    | GND             |

> ⚠️ Ensure both devices share a common ground.

---

## ⚙️ SPI Configuration

- Mode: Master (STM32)
- Communication: Full Duplex
- Data Frame: 8-bit
- Clock Speed: fPCLK / 8
- CPOL: Low
- CPHA: Low
- NSS: Hardware (SSOE enabled)

---

## 📡 Command Protocol

| Command            | Code  | Description              |
|-------------------|------|--------------------------|
| LED Control       | 0x50 | Turn LED ON/OFF          |
| Sensor Read       | 0x51 | Read analog value        |
| LED Read          | 0x52 | Get LED status           |
| Print Message     | 0x53 | Send string              |
| ID Read           | 0x54 | Read device ID           |

---

## 🔁 Communication Flow

1. Master sends command
2. Slave responds with ACK (0xF5)
3. Master sends arguments (if required)
4. Slave processes request and sends response

---

## 📍 Functional Overview

### LED Control
Controls LED state on Arduino using SPI command.

### Sensor Read
Reads analog values from Arduino and sends back to STM32.

### LED Status
Retrieves ON/OFF status of LED.

### Print Message
Sends a string from STM32 to Arduino.

### ID Read
Reads a fixed device ID from Arduino.

---

## 🧩 Driver Implementation

### GPIO Driver
- Pin configuration
- Input/output operations
- External interrupt support

### SPI Driver
- Peripheral initialization
- Data transmission (blocking)
- Flag-based synchronization (TXE, RXNE, BSY)
- NSS hardware management

---

## ▶️ How to Run

1. Import project into STM32 IDE (CubeIDE / Keil / Makefile)
2. Flash STM32F401
3. Upload SPI slave code to Arduino UNO
4. Connect hardware as per table
5. Press button (PA0) to trigger SPI commands

---

## ⚠️ Notes

- Dummy read/write is required for SPI synchronization
- Always verify ACK before sending data
- Ensure SPI settings match on both devices
- Proper NSS handling is critical

---

## 🧪 Debug Tips

- No communication:
  - Check wiring and grounding
  - Verify SPI clock enable
  - Confirm CPOL/CPHA settings

- Incorrect data:
  - Check dummy byte handling
  - Ensure correct data frame size (8-bit)

---

## 📈 Future Improvements

- Interrupt-based SPI communication
- DMA support
- Error handling (OVR, MODF)
- RTOS integration
- Multi-device SPI support

---

## 🤝 Contribution
Feel free to fork this repository and contribute improvements.

---

## 📄 License
This project is licensed under the MIT License.
