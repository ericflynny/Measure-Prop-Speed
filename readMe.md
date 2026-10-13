# Measurement and Transmission of Propeller Speed

Eric Flynn, Fall 2026

EN.605.715: Software Development For Real-Time Embedded Systems, Johns Hopkins University

## About
The following project will:
1. Measure propeller speed (RPM) using an optical IR emitter/detector pair on external interrupt Pin 2
2. Toggle diagnostic Pin 7 (Port H, Pin 4) via Direct Port Manipulation (62.5ns latency) on ISR entry/exit to measure software latency
3. Measure propeller RPM driven by external motor supply (Analog Discovery 2 Wavegen / Power Supply)
4. Transmit tachometer data natively over USB-Serial in CSV format at 115200 baud (10 Hz rate)
5. Mirror optical sensor state directly to onboard LED (Pin 13)
6. Support dual-compilation using either standard Arduino IDE (`setup`/`loop`) or VS Code PlatformIO (standard C++ `main`)

## Hardware Connections
- **IR Phototransistor Collector (Long leg)** -> Arduino Pin 2 (with 10kΩ pull-up to 5V)
- **IR Phototransistor Emitter (Short leg)** -> Arduino GND
- **IR LED Anode (Long leg)** -> Arduino 5V (with 220Ω series resistor)
- **IR LED Cathode (Short leg)** -> Arduino GND
- **External Motor Drive** -> Power Supply
- **Diagnostic Debug Pin** -> Arduino Pin 7 (Port H, Pin 4)


## Steps to Compile and Program Arduino

This project is structured to support **both** VS Code (PlatformIO) and the standard Arduino IDE.

### Option A: VS Code with PlatformIO (Recommended)
1. Open this project folder in VS Code
2. Connect the Arduino Mega 2560 to your host PC via USB
3. Click the PlatformIO **Upload** button (the right arrow `→` on the bottom blue status bar)

### Option B: Standard Arduino IDE
1. Open `PropSpeed/PropSpeed.ino` in the Arduino IDE. (The IDE will automatically pull in the core files from its `src/` folder).
2. Connect the Arduino Mega 2560 to your host PC
3. At the top of the Arduino IDE, select the connected Arduino board and serial port
4. File -> Upload (This will compile and upload the firmware to the Arduino)

## Python Automated Logger & HTML Graph (For Experiments)
Run the Python logger script on your host machine to automatically read the serial port, log data directly to a `.csv` file, and render a live-updating web graph.

```bash
# Log live data from Arduino to rpm_data.csv and generate rpm_graph.html
python3 speedLogger.py

# Optional: Resume/append to an existing session
python3 speedLogger.py --append
```
- Open `rpm_graph.html` in Safari or Chrome to view an interactive graph. It refreshes automatically.
- All data points are saved in `rpm_data.csv`.

*Note: Make sure to close any Serial Monitors before running the script so the script can access the port.*

## Acknowledgments
*Note: The Antigravity AI coding assistant (Google) was utilized to help refactor and document portions of this project.*
