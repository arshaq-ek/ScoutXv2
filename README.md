# ScoutXv2 - Indoor Surveillance & Delivery Robot  

ScoutXv2 is an advanced indoor surveillance and delivery robot utilizing an **ESP32-CAM** and **Arduino Mega**. It features **real-time video streaming, Bluetooth motor control, and facial recognition-based automation**, making it suitable for smart home security and industrial applications.  

## Features  
- 📷 **Live Video Streaming** – View real-time footage via the ESP32-CAM web interface.  
- 🎮 **Manual & Autonomous Movement** – Control the bot using web buttons or facial recognition.  
- 🤖 **Facial Recognition & Tracking** – Detect and follow recognized faces.  
- 🔗 **ESP32-CAM & Arduino Mega Communication** – Uses HC-05 Bluetooth for motor commands.  
- ⚡ **Servo Mechanism** – Opens/closes a compartment when a face is detected.  
- 🌐 **Wi-Fi AP Mode** – ESP32-CAM hosts its own Wi-Fi network for direct access.  

## Hardware Components  
- **ESP32-CAM AI-Thinker** – For video streaming & face recognition.  
- **Arduino Mega 2560** – Handles motor control & command execution.  
- **HC-05 Bluetooth Module** – Enables ESP32 ↔ Arduino communication.  
- **L298N Motor Driver** – Controls DC motors.  
- **12V DC Motors & Servo Motor** – For movement and object interaction.  
- **Power Supply** – 12V Battery & 5V Distribution PCB.  

## Software & Dependencies  
- **Arduino IDE** – For ESP32-CAM & Arduino Mega programming.  
- **ESP32-CAM CameraWebServer Example** – Modified for custom UI & controls.  
- **Python & OpenCV** (optional) – For PC-based real-time image processing.  

## Installation & Setup  
### 1️⃣ ESP32-CAM Setup  
- Flash the modified **CameraWebServer** example to the ESP32-CAM.  
- Connect to its Wi-Fi AP and access the stream via a web browser.  

### 2️⃣ Arduino Mega Setup  
- Upload the **motor control sketch** to Arduino Mega.  
- Connect via HC-05 Bluetooth to receive movement commands.  

### 3️⃣ Testing & Debugging  
- Open the ESP32-CAM web interface to control the robot.  
- Monitor the Serial output for debugging.  

## Future Improvements  
✔️ **Obstacle Avoidance** with ultrasonic sensors.  
✔️ **Path Planning & SLAM Integration** for full autonomy.  
✔️ **Cloud Connectivity** for remote control & monitoring.  

## Contributors  
👨‍💻 **[Your Name]** – Lead Developer & System Architect  
🔗 **[Other Contributors]** (if applicable)  

## License  
📜 This project is open-source under the **MIT License**. Feel free to modify and improve it!  

---

🚀 **Follow for more updates!**
