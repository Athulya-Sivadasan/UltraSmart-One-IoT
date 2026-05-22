# 🌆 UltraSmart One
### A Multifunctional Device for Smart Urban Solutions

> An integrated IoT-based smart city system that automates water management, parking control, and waste monitoring — built on ESP32.

---

## 📌 About This Project

UltraSmart One is a real-time IoT system developed as a Mini Project for APJ Abdul Kalam Technological University (March 2026). It addresses three major urban challenges simultaneously using a single ESP32-based platform:

- 💧 Water wastage due to manual monitoring
- 🚗 Inefficient parking and traffic congestion  
- 🗑️ Improper waste disposal and overflow

The system hosts a **local web dashboard** via Wi-Fi Access Point — no external internet connection required.

---

## 🧩 System Modules

### 💧 Water Level Management
- Ultrasonic sensor monitors tank level continuously
- Pump AUTO ON when level drops below threshold (>8 cm distance)
- Pump AUTO OFF when tank is full — prevents overflow and saves electricity

### 🚗 Smart Parking System
- IR sensor detects vehicle at entrance gate
- Servo motor opens/closes gate automatically
- Two ultrasonic sensors monitor parking slot occupancy
- Green LED = slot available · Red LED = parking full

### 🗑️ Waste Management System
- Ultrasonic sensor tracks garbage bin fill level
- Buzzer alert triggers when bin is full (<8 cm distance)
- Status updated on web dashboard in real time

---

## ⚙️ Hardware Components

| Component | Quantity | Purpose |
|---|---|---|
| ESP32 Microcontroller | 1 | Central unit + Wi-Fi Access Point |
| HC-SR04 Ultrasonic Sensor | 4 | Water level, 2 parking slots, waste bin |
| IR Sensor | 1 | Vehicle detection at entrance |
| Servo Motor | 1 | Automatic gate control |
| Relay Module | 1 | Controls water pump via ESP32 |
| BC547 NPN Transistor | 2 | Drives relay and buzzer safely |
| LED (Green + Red) | 2 | Parking status indicators |
| Buzzer | 1 | Waste bin full alert |
| Water Pump | 1 | Automated water filling |
| Perfboard | 1 | Permanent circuit assembly |
| Resistors (220Ω, 1kΩ) | Multiple | Current limiting |
| 5V DC Battery | 1 | Power supply |

---

## 💻 Software & Tools

- **Arduino IDE** — Firmware development
- **WiFi.softAP()** — ESP32 as Wi-Fi Access Point
- **WebServer library** — Hosts dashboard on port 80
- **ESP32Servo library** — Servo control via PWM signals
- **JSON API** (`/api/data`) — Real-time sensor data to web interface
- **millis()** — Non-blocking loop for smooth performance
- **HTML + CSS + JavaScript** — Web dashboard UI

---

## 🌐 System Architecture

```
Sensors (HC-SR04 × 4, IR Sensor)
            ↓
    ESP32 reads & processes data
            ↓
  Threshold-based logic triggers actions
            ↓
  Controls: Water Pump | Servo Gate | Buzzer | LEDs
            ↓
  JSON data → Local Web Server (Wi-Fi AP)
            ↓
      Web Dashboard (Browser)
```

---

## 📊 Working Conditions

| Scenario | Sensor Input | Action |
|---|---|---|
| Parking slot free | Distance > 10 cm | Green LED ON |
| Parking full | Both slots < 10 cm | Red LED ON, Gate closed |
| Car at gate | IR = LOW | Servo opens gate |
| Low water level | Tank distance > 8 cm | Pump ON |
| Tank full | Tank distance ≤ 8 cm | Pump OFF |
| Waste bin full | Bin distance < 8 cm | Buzzer alert |

---

## 🚀 How to Run This Project

1. Clone this repository
2. Open the code in **Arduino IDE**
3. Install required libraries:
   - `ESP32Servo`
   - `WebServer` (built-in with ESP32 board package)
4. Connect hardware as per the circuit diagram
5. Upload code to ESP32
6. Connect your phone/laptop to Wi-Fi network: `SmartCityPro`
7. Open browser and go to: `192.168.4.1`
8. View the live dashboard!

---

## 👩‍💻 Team Members

| Name | Roll No |
|---|---|
| Aju Fathima T | ATP23EC004 |
| Anu L | ATP23EC011 |
| Astuti Kumari Jha | ATP23EC014 |
| Athulya Sivadasan | ATP23EC016 |
| Meenu Mol S | ATP23EC026 |

**Guide:** Ms. Gayathri P S  
**Department:** Electronics and Communication Engineering  
**Institution:** Ahalia School of Engineering and Technology, Palakkad  
**University:** APJ Abdul Kalam Technological University

---

## 🔮 Future Scope

- Mobile app for remote monitoring and control
- AI-based prediction of water usage and parking demand
- Solar power integration for sustainability
- Cloud-based data analysis and multi-unit deployment
- GPS-based tracking and automated parking billing

---

## 📄 License

This project was submitted for academic purposes under APJ Abdul Kalam Technological University.
