
# embedded-json-serializer
A lightweight, platform-agnostic C library for serializing embedded data structures into JSON, with an Arduino-based demonstration using UART.
# Embedded JSON Serializer

## Overview

This project implements a lightweight, embedded-friendly JSON serialization library designed for smart meter / w-M-Bus–related firmware pipelines.

The library takes structured measurement and gateway metadata stored in C data structures and serializes it into a **strictly defined JSON format**. The output is intended to be forwarded to a gateway, backend system, or higher-level communication layer.

The scope of this assignment is **intentionally limited to serialization only**. Radio communication (w-M-Bus), OMS protocol handling, encryption, networking, or real meter interfacing are **out of scope**. This reflects real embedded firmware design, where responsibilities are cleanly separated into layers.

The implementation focuses on:

* Clean software architecture
* Predictable memory usage
* Clear API design
* Portability across embedded platforms

---

## Features

* Pure C implementation suitable for embedded systems
* No dynamic memory allocation 
* No external JSON libraries
* Transport-agnostic (no UART, radio, or network code inside the library)
* Caller-provided output buffer to prevent memory fragmentation
* Strict adherence to the required JSON schema
* Explicit error handling (invalid input, buffer overflow, formatting errors)
* Configurable limits for number of devices and data points

---

## Data Model

The internal data model mirrors the required JSON structure while remaining efficient for embedded use.

### Key Concepts

* **Static allocation** is used instead of linked lists or heap allocation.
* Maximum sizes are defined at compile time using macros.
* Strings are represented as const char * to avoid copying and reduce RAM usage.

### Main Structures

* **JsonMeterDataPoint**
  Represents a single meter reading entry (one element inside the data array). It contains timestamps, a numeric meter value, and a status string.

* **JsonDeviceReading**
  Represents one device. Each device can contain multiple data points. Devices are stored in a fixed-size array.

* **JsonValues**
  Groups all device readings and keeps track of how many devices are currently used.

* **JsonGateway**
  Top-level structure holding gateway metadata and the associated measurement values. One JsonGateway corresponds to one object inside the outer JSON array.

This structure allows the library to support **multiple devices**, each with **multiple data points**, while remaining deterministic in memory usage.

---

## Library API

The library exposes a small and focused public API.

### Serialization Function

* Accepts a pointer to a populated JsonGateway structure
* Writes the serialized JSON into a caller-provided character buffer
* Requires the caller to specify the buffer size

The function:

* Does **not** perform any I/O
* Does **not** allocate memory
* Returns explicit error codes instead of printing messages

### Maximum Size Query

The library provides a helper function to calculate the **maximum possible JSON size** based on configured limits. This allows the application to safely allocate an output buffer.

This API design ensures:

* Safe operation on memory-constrained systems
* Easy integration with any transport layer
* Clear separation between data preparation, serialization, and transmission

---

## Demo Application

The demo application is written using the Arduino framework and serves as a **usage example**, not as part of the core library.

The demo:

1. Creates fake gateway and meter data
2. Populates the library data structures
3. Calls the serialization function
4. Prints the resulting JSON to a serial interface

The demo also prints human-readable status messages so the serialization process is easy to follow during simulation or video demonstration.

All formatting, printing, and UART handling is intentionally kept **outside** the library to demonstrate transport-agnostic design.

---

## Simulation Environment

The project is demonstrated using:

* **Arduino Mega 2560** (AVR-based)
* **Proteus** for simulation
* **Virtual Terminal** for UART output

### UART Configuration

* Serial interface: `Serial (UART0)
* Baud rate: 9600
* Virtual Terminal RX connected to Arduino TX0 (D1)
* Virtual Terminal TX connected to Arduino RX0 (D0)

Proteus requires a short startup delay and a manual reset after simulation start to ensure UART output is visible.

---

## Output Example

```json
[
  {
    "gatewayId": "gateway_1234",
    "date": "1970-01-01",
    "deviceType": "stromleser",
    "interval_minutes": 15,
    "total_readings": 1,
    "values": {
      "device_count": 1,
      "readings": [
        {
          "media": "water",
          "meter": "waterstarm",
          "deviceId": "stromleser_50898527",
          "unit": "m3",
          "data": [
            {
              "timestamp": "1970-01-01 00:00",
              "meter_datetime": "1970-01-01 00:00",
              "total_m3": 107.752,
              "status": "OK"
            }
          ]
        }
      ]
    }
  }
]
```

The actual library output is compact (single-line JSON). Pretty formatting is applied only for demonstration and documentation purposes.

---

## Video Demonstration

A short demonstration video is provided showing:

* Project structure
* Data model explanation
* Proteus simulation setup
* Live JSON output on the virtual terminal

https://youtu.be/X16jDGoxdO0

---

## Platform Notes

* Arduino / AVR: Fully supported and used for demonstration
* STM32: **Compatible** (pure C, no Arduino-specific dependencies in library)
* ESP32: Compatible with minor integration changes

The library avoids platform-specific features and can be reused in bare-metal or RTOS-based systems.

The Arduino (AVR-based) platform was chosen for this assignment primarily due to cost efficiency, predictability, and hardware-level control, which are important factors in real-world smart meter product design.

When designing an actual PCB for a large-scale deployment such as residential smart meters, AVR microcontrollers are significantly cheaper than STM32 or ESP32-class devices. This makes them attractive for cost-sensitive products where margins and volume matter.

Additionally, AVR microcontrollers are 8-bit CPUs with highly predictable timing behavior. Compared to more complex 32-bit MCUs, they offer simpler execution models, fewer abstraction layers, and closer interaction with the hardware. This predictability is beneficial in metering and control-oriented applications, where deterministic behavior is often more important than raw computational performance.

Hardware selection ultimately depends on the nature of the expected data and system requirements. In typical residential smart metering scenarios, readings are periodic and relatively slow-changing. In such cases, an AVR-based solution is sufficient and efficient. If the application were to involve highly fluctuating or high-frequency measurements, a more powerful platform such as an STM32 would be a better choice due to its higher processing capability and peripheral performance.

Regarding future IoT or connectivity upgrades, AVR-based systems do not natively provide wireless connectivity. In a real product design, this would be addressed at the PCB level by integrating external communication modules (e.g., Bluetooth, sub-GHz radio, or other wireless interfaces) as required. The current demonstration therefore focuses on core firmware architecture and serialization logic, independent of the communication medium.

**Note: The library can be used for STM32 and ESP32(with subtle changes) since the library is written purely in C, I have used Arduino on Proteus only for demonstration purpose.** 
