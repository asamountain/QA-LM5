# 📡 LM5/BG96 LTE Signal Monitor (QA Tool)

A robust C++ utility designed for Quality Assurance (QA) testing of Quectel LTE Cat M1/NB-IoT modems (BG96/LM5).

This tool moves beyond simple "Signal Strength" (RSSI) to monitor the critical "Vital Signs" of an LTE connection: **RSRP**, **SINR**, and **RSRQ**. It provides real-time visualization on the console with a live scrolling dashboard.

---

## 🚀 Key Features

* **Comprehensive Metrics:** Monitors RSSI, RSRP (True Signal), SINR (Interference), and RSRQ (Quality).
* **Live Dashboard:** Enhanced visual dashboard with scrolling history tracking (last 15 readings) and color-coded signal quality indicators.
* **Real-Time Visualization:** Displays ASCII-based "Health Bars" for instant visual feedback during drive tests or enclosure testing.
* **Auto-Logging:** Automatically saves all data points to `lte_signal_log.csv` with precise timestamps.
* **Robust Connectivity:** Includes "Auto-Reconnect" logic. If the USB cable is bumped or the modem restarts, the tool pauses and reconnects automatically without crashing.
* **Non-Blocking I/O:** Uses smart serial polling to prevent the "100-second freeze" issue common in basic serial tools. 
* **Interactive Terminal:** Includes Python script (`modem_test.py`) for direct AT command interaction with the modem.

---

## 🛠️ Hardware Requirements

* **Host:** Linux-based system (Raspberry Pi 3/4/5, Ubuntu PC, etc.)
* **Modem:** Quectel BG96 / LM5 (or compatible LTE Cat M1/NB1 module)
* **Connection:** USB Serial (typically `/dev/ttyUSB2` or `/dev/ttyUSB7`)

## 📦 Software Prerequisites

### For C++ Monitor Tool
* `g++` (GCC Compiler)
* Standard C++ Libraries (included in most Linux distributions)

### For Python Interactive Terminal
* Python 3.x
* `pyserial` library

To install prerequisites on Raspberry Pi/Ubuntu: 

```bash
sudo apt-get update
sudo apt-get install build-essential python3 python3-pip
pip3 install pyserial
```

---

## ⚙️ Installation & Compilation

### Advanced Monitor (C++)

1. **Download the Source Code**
   Clone the repository or download `advanced_monitor.cpp`.
   
2. **Verify Serial Port**
   Check which port your modem uses for AT commands (usually USB2 or USB7).
   ```bash
   ls /dev/ttyUSB*
   ```

   *If your port is different from `/dev/ttyUSB7`, edit the `PORT_NAME` line in the C++ file.*
   
3. **Compile the Tool**
   Run the following command to compile the C++ code into an executable:
   ```bash
   g++ -o advanced_monitor advanced_monitor.cpp
   ```

### Interactive Terminal (Python)

The `modem_test.py` script is ready to use without compilation. Just ensure you have the correct serial port configured.

---

## 🖥️ Usage

### Option 1: Advanced Monitor (Recommended for QA Testing)

Run the tool with `sudo` privileges (required to access serial ports):

```bash
sudo ./advanced_monitor
```

#### **Live Dashboard Output:**

```text
========================================================================================
     LM5 LTE SIGNAL MONITOR - LIVE DASHBOARD  
========================================================================================
 METRIC | GOOD RANGE      | WHY IT MATTERS?
--------+-----------------+-------------------------------------------------------------
 RSSI   | > -85 dBm       | Total Noise. Ignored if RSRP is bad. 
 RSRP   | > -105 dBm      | True Signal Strength. If low (-120), antenna is disconnected.
 SINR   | > 5 dB          | Signal Clarity. High = Fast.  Low = Noisy/Laggy.
 RSRQ   | > -15 dB        | Quality. Low (-20) means the tower is congested.
========================================================================================
 TIMESTAMP            | RSSI  | RSRP       | SINR     | RSRQ  | VISUAL CHART       
----------------------+-------+------------+----------+-------+--------------------
 2026-01-21 05:36:20  | -53   | -79 dBm    | 19 dB    | -8    | [#####] Excellent  
 2026-01-21 05:36:21  | -54   | -80 dBm    | 18 dB    | -9    | [#####] Excellent  
 2026-01-21 05:36:22  | -54   | -81 dBm    | 18 dB    | -10   | [#####] Excellent  
----------------------------------------------------------------------------------------
 Saving log to:  lte_signal_log.csv  (Press Ctrl+C to stop)
```

### Option 2: Interactive Terminal (For Manual Testing)

Use the Python script to send custom AT commands directly to the modem:

```bash
python3 modem_test.py
```

This allows you to: 
- Send custom AT commands
- Test modem responses
- Debug connection issues
- Experiment with different modem configurations

### **Stopping the Tools**

Press `Ctrl + C` to stop monitoring or exit the interactive terminal.

---

## 📊 How to Interpret the Data (QA Guide)

| Metric | Full Name | Meaning | QA Target |
| --- | --- | --- | --- |
| **RSRP** | Reference Signal Received Power | **True Signal Strength.** The volume of the tower's voice. Ignore RSSI; trust this number. | **> -105 dBm** (Higher is better) |
| **SINR** | Signal to Interference & Noise Ratio | **Signal Clarity.** High positive numbers mean a clear connection.  Negative numbers mean noise is louder than the signal. | **> 5 dB** (Higher is better) |
| **RSRQ** | Reference Signal Received Quality | **Voice Quality.** Measures congestion and fluidity. | **> -15 dB** (Higher is better) |
| **RSSI** | Received Signal Strength Indicator | **Total Noise.** Can be misleading.  High RSSI + Low SINR = Bad Interference. | N/A (Context dependent) |

### Signal Quality Indicators

The visual chart provides quick assessment:
- `[#####] Excellent` - RSRP > -80 dBm (Strong signal)
- `[####]  Good` - RSRP > -90 dBm (Acceptable for most uses)
- `[###]   Fair` - RSRP > -105 dBm (Usable but marginal)
- `[##]    Poor` - RSRP > -115 dBm (Unreliable connection)
- `[#]     Critical` - RSRP > -140 dBm (Connection failing)
- `[!]     No Signal` - RSRP < -140 dBm or 0 (No connection)

### **Diagnosis Cheat Sheet:**

* **Low RSRP (-120), Low SINR (0):** Antenna is disconnected or broken.
* **High RSSI (-60), Low SINR (-5):** High interference environment (noisy electronics).
* **Good RSRP (-80), Low SINR (0):** Congested network or internal board noise. 
* **Fluctuating values:** Physical obstruction or signal reflection (multipath).

---

## 📂 Log File Analysis

The tool generates `lte_signal_log.csv` in the same directory.
You can import this file directly into **Microsoft Excel** or **Google Sheets** to create performance graphs (Time vs. RSRP) for test reports.

**CSV Format:**

```csv
Timestamp,RSSI,RSRP,SINR,RSRQ,Quality_Status
2026-01-21 05:36:20,-53,-79,19,-8,[#####] Excellent
2026-01-21 05:36:21,-54,-80,18,-9,[#####] Excellent
```

### Recommended Analyses: 
- **Time-series graphs** of RSRP to identify signal degradation patterns
- **SINR vs. RSRP scatter plots** to identify interference sources
- **RSRQ trends** to detect network congestion during peak hours
- **Min/Max/Average calculations** for QA acceptance criteria

---

## 🔧 Configuration Options

### Advanced Monitor Settings

Edit the constants at the top of `advanced_monitor.cpp`:

```cpp
const char* PORT_NAME = "/dev/ttyUSB7";  // Change to your serial port
const int BAUD_RATE = B115200;           // Serial baud rate
const char* LOG_FILE_NAME = "lte_signal_log.csv";
const int HISTORY_SIZE = 15;             // Number of readings shown in dashboard
```

After making changes, recompile: 
```bash
g++ -o advanced_monitor advanced_monitor.cpp
```

---

## ⚠️ Troubleshooting

**1. "Connection Failed" / "[CONNECTING] Trying to open port..."**

* Check if the USB cable is plugged in.
* Verify the port name: `ls /dev/ttyUSB*`.
* Ensure you are running with `sudo`.
* Check if another program is using the serial port.

**2. Data stops updating**

* This tool is designed to auto-reconnect. If it stops for >10 seconds, the modem itself may have rebooted or lost power. 
* Check USB power supply (some Raspberry Pi models have limited USB current).

**3. "Permission Denied"**

* You must run the tool as root: `sudo ./advanced_monitor`.
* Alternatively, add your user to the `dialout` group: 
  ```bash
  sudo usermod -a -G dialout $USER
  ```
  (Requires logout/login to take effect)

**4. Incorrect readings or parse errors**

* Use the interactive terminal (`modem_test.py`) to verify modem responses manually.
* Some modem firmware versions may format AT command responses differently. 
* Check that you're using the correct AT command for your modem model.

**5. Python script issues**

* Verify pyserial is installed: `pip3 list | grep serial`
* Check Python version: `python3 --version` (requires 3.x)
* Ensure the serial port path is correct in the script.

---

## 📈 What's New

### v2.0 (January 2026)
- ✨ **Enhanced Dashboard**: Added scrolling history with last 15 readings
- 📊 **Improved Layout**: Better column alignment and visual clarity
- 🔄 **Auto-Reconnect Display**: Shows connection status in real-time
- 🐍 **Python Terminal**: Added `modem_test.py` for interactive AT command testing
- 📝 **Better Documentation**:  Expanded troubleshooting and configuration sections

### v1.0 (January 2026)
- 🎉 Initial release
- Basic signal monitoring with RSSI, RSRP, SINR, RSRQ
- CSV logging functionality
- Auto-reconnect feature

---

## 📄 License

This project is open-source and available for use in QA testing and development environments.

## 🤝 Contributing

Contributions, issues, and feature requests are welcome!  Feel free to check the issues page or submit pull requests.

## 👤 Author

**Seungjin Youn** (asamountain)

---

**Need Help?** Open an issue on GitHub or check the troubleshooting section above. 
