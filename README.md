# ❄️ Cold Chain Monitoring Gateway

RTOS-based IoT embedded system for real-time temperature monitoring using ESP32, Zephyr RTOS, BLE, and AWS IoT.

---

## 📌 Project Overview

This project implements an end-to-end cold chain monitoring solution for pharmaceutical and food storage environments.

The system consists of:

- ESP32 Sensor Node (Zephyr RTOS)
- ESP32 Gateway Node (Arduino)
- AWS IoT Cloud
- BLE communication
- MQTT over TLS

Sensor data is collected, locally processed, transmitted wirelessly, and securely published to AWS IoT.

---

## 🧱 System Architecture

ESP32 Slave → BLE → ESP32 Gateway → WiFi → AWS IoT → Dashboard

---

## 📁 Repository Structure

### 🔹 ColdChain_Slave
Zephyr RTOS firmware for sensor node.

- src/ → Application source
- include/ → Header files
- CMakeLists.txt → Build config
- prj.conf → Zephyr kernel config

Features:
- Timer-based sensing
- Threshold control
- BLE GATT notifications
- Health monitoring

---

### 🔹 ColdChain_Master
Arduino ESP32 gateway firmware.

- BLE client
- WiFi connection
- TLS authentication
- MQTT publishing to AWS

---

### 🔹 ColdChain_Dashboard

Contains AWS certificates:

- Root CA
- Device Certificate
- Private Key

Used for secure MQTT connection.

---

### 🔹 Documentation

- Project presentation
- Final report

---

## 🚀 Build Slave Node (Zephyr)

```bash
west build -b esp32 . -p always
west flash
west espressif monitor
