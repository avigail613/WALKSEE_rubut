# WALKSEE_rubut
# 🦯 WalkSee – Autonomous Navigation & Obstacle Avoidance Cane

An advanced Embedded and IoT assistive technology system designed to empower visually impaired individuals with safe, independent, and intuitive urban navigation.



![אנימציה רצה](הקישור_שגיטהאב_יצר_עבור_ה_GIF_שלך.gif)

---
### 🔗 קישורים לצפייה בסרטונים המלאים:
* [לצפייה בסרטון המלא ביוטיוב](הקישור_לסרטון_הראשון)
* [לצפייה בסרטון המלא בדרייב](הקישור_לסרטון_השני)

![אנימציה רצה](https://github.com/user-attachments/assets/0d441968-f1f3-4fd1-8fdc-7ad938eb3683.gif)
<video
    autoplay
    muted
    loop
    playsinline
    disablepictureinpicture
    controlslist="nodownload noplaybackrate nofullscreen"
    style="
        position: fixed;
        inset: 0;
        width: 100%;
        height: 100%;
        object-fit: cover;
        pointer-events: none;
        z-index: -1;
    ">
    <source src="background.webm" type="video/webm">
</video>

[סרטו הסבר קצר (online-video-cuttercom) (video-converter.com).webm](https://github.com/user-attachments/assets/1fac63c2-a637-4055-b917-6690baa1556e)


---

## 📌 Project Overview
**WalkSee** simulates a human guide. Instead of requiring high cognitive load or screen interaction, the smart robotic cane physically guides the user forward, dynamically avoiding obstacles and providing real-time audio instructions via Bluetooth.

The system features:
* **Physical Robotic Guidance:** A motorized chassis pulling the cane in the correct direction.
* **Intelligent Obstacle Avoidance:** Multi-angle scanning using LiDAR and IR sensors.
* **Voice-Activated Destination Entry:** Speech-to-text destination parsing powered by Cloud AI.
* **Real-time Web Monitoring:** Live GPS tracking on an interactive map for family members or guardians.

---

## 🧠 System Architecture & Data Flow

```text
[ User Speech ] ──> [ INMP441 Mic ] ──> [ SD Card (WAV) ] ──> [ ESP32 (WiFi) ]
                                                                     │
  ┌──────────────────────────────────────────────────────────────────┘
  ▼
[ Python/Node.js Server ] ──> [ AI Speech-to-Text API ] ──> [ Route Calculation (Google Maps) ]
  │
  ├─> [ SQL Server / Database ] (Saves route history & real-time GPS)
  │
  └─> [ ESP32 Client ] ──> [ Motors (Movement) ] & [ Bluetooth A2DP (Audio Instructions) ]
```
### 🔧 Hardware & Component Breakdown
The hardware client is built around the ESP32 microcontroller, managing:  
**Central Controller**: ESP32 Development Board (Dual-core, WiFi/Bluetooth built-in).  
**Obstacle Detection**: TFmini-S LiDAR (Laser ToF sensor for distance) & MH-Sensor IR. 
**Localization**: u-blox NEO-6M GPS Module for real-time tracking. 
**Audio Input**: INMP441 MEMS digital microphone (I2S protocol).  
**Audio Output**: Bluetooth A2DP Source streaming audio to wireless headphones. 
**Actuation**: DC Motors driven by an L298N H-Bridge Motor Driver.  
### 💻 Software Stack
**Embedded firmware**: C++ (Arduino IDE / FreeRTOS).  
**Backend Server**: Node.js / Python Flask.  
**Database:** SQL Server. 
**Frontend Dashboard:** React.js, HTML5, CSS3.
### 🚀 Key Engineering Challenges Solved
#### 1. On-Board Voice Interface without Screen Interactions
**The Challenge**: Visually impaired users cannot use screens to input destinations.  
**The Solution**: Integrated an I2S MEMS microphone, buffered the raw audio data onto an SD card, and developed a multipart HTTP post-mechanism to stream the file to a cloud API for natural language processing. 
#### 2. High-Performance Wireless Audio Integration
**The Challenge**: Finding a lightweight, hands-free way to output instructions in noisy street environments without cluttered wires[cite: 2].
The Solution: Leveraged the ESP32's built-in Bluetooth controller with the BluetoothA2DPSource library, converting local WAV files into dynamic binary buffers streamed directly to standard Bluetooth headphones[cite: 2].
#### ⚡ Getting Started (Installation & Setup)
**1. Flash the ESP32**
Firmware:Open the firmware/ folder in Arduino IDE[cite: 2].
Ensure WiFi.h, TinyGPS++.h, SD.h, and BluetoothA2DPSource.h are installed[cite: 2].
Configure your local SSID and Server Endpoint[cite: 2].
Upload to your ESP32 board[cite: 2].
**2. Run the Backend Server:**
```
cd server
npm install   # or pip install -r requirements.txt
npm start     # or python server.py
```
**3. Launch the React Dashboard:**
```
cd client
npm install
npm run dev
```

**
