# 📡 LM5/BG96 LTE Signal Monitor (QA Tool)

A robust C++ utility designed for Quality Assurance (QA) testing of Quectel LTE Cat M1/NB-IoT modems (BG96/LM5).

This tool moves beyond simple "Signal Strength" (RSSI) to monitor the critical "Vital Signs" of an LTE connection: **RSRP**, **SINR**, and **RSRQ**. It provides real-time visualization on the console and logs all data to a CSV file for post-test analysis.

---

## 🚀 Key Features

* **Comprehensive Metrics:** Monitors RSSI, RSRP (True Signal), SINR (Interference), and RSRQ (Quality).
* **Real-Time Visualization:** Displays ASCII-based "Health Bars" for instant visual feedback during drive tests or enclosure testing.
* **Auto-Logging:** Automatically saves all data points to `lte_signal_log.csv` with precise timestamps.
* **Robust Connectivity:** Includes "Auto-Reconnect" logic. If the USB cable is bumped or the modem restarts, the tool pauses and reconnects automatically without crashing.
* **Non-Blocking I/O:** Uses smart serial polling to prevent the "100-second freeze" issue common in basic serial tools.

---

## 🛠️ Hardware Requirements

* **Host:** Linux-based system (Raspberry Pi 3/4/5, Ubuntu PC, etc.)
* **Modem:** Quectel BG96 / LM5 (or compatible LTE Cat M1/NB1 module)
* **Connection:** USB Serial (typically `/dev/ttyUSB2` or `/dev/ttyUSB7`)

## 📦 Software Prerequisites

* `g++` (GCC Compiler)
* Standard C++ Libraries (included in most Linux distributions)

To install g++ on Raspberry Pi/Ubuntu:

```bash
sudo apt-get update
sudo apt-get install build-essential

```

---

## ⚙️ Installation & Compilation

1. **Download the Source Code**
Save the code as `advanced_monitor.cpp`.
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



---

## 🖥️ Usage

Run the tool with `sudo` privileges (required to access serial ports):

```bash
sudo ./advanced_monitor

```

### **Console Output Example:**

```text
LM5 Full QA Monitor (RSSI + RSRP + SINR + RSRQ)
Port: /dev/ttyUSB7 | Log: lte_signal_log.csv
---------------------------------------------------------------------------------------
TIMESTAMP           | RSSI  | RSRP (True Signal)    | SINR | RSRQ  | VISUAL CHART
---------------------------------------------------------------------------------------
2026-01-20 16:42:20 | -53   | -79 dBm [#####] Excellent | 19 dB | -8  | 
2026-01-20 16:42:21 | -54   | -80 dBm [#####] Excellent | 18 dB | -9  | 
2026-01-20 16:42:22 | -54   | -81 dBm [#####] Excellent | 18 dB | -10 | 

```

### **Stopping the Tool**

Press `Ctrl + C` to stop monitoring.

---

## 📊 How to Interpret the Data (QA Guide)

| Metric | Full Name | Meaning | QA Target |
| --- | --- | --- | --- |
| **RSRP** | Reference Signal Received Power | **True Signal Strength.** The volume of the tower's voice. Ignore RSSI; trust this number. | **> -105 dBm** (Higher is better) |
| **SINR** | Signal to Interference & Noise Ratio | **Signal Clarity.** High positive numbers mean a clear connection. Negative numbers mean noise is louder than the signal. | **> 5 dB** (Higher is better) |
| **RSRQ** | Reference Signal Received Quality | **Voice Quality.** Measures congestion and fluidity. | **> -15 dB** (Higher is better) |
| **RSSI** | Received Signal Strength Indicator | **Total Noise.** Can be misleading. High RSSI + Low SINR = Bad Interference. | N/A (Context dependent) |

**Diagnosis Cheat Sheet:**

* **Low RSRP (-120), Low SINR (0):** Antenna is disconnected or broken.
* **High RSSI (-60), Low SINR (-5):** High interference environment (noisy electronics).
* **Good RSRP (-80), Low SINR (0):** Congested network or internal board noise.

---

## 📂 Log File Analysis

The tool generates `lte_signal_log.csv` in the same directory.
You can import this file directly into **Microsoft Excel** or **Google Sheets** to create performance graphs (Time vs. RSRP) for test reports.

**CSV Format:**

```csv
Timestamp,RSSI,RSRP,SINR,RSRQ,Quality_Status
2026-01-20 16:42:20,-53,-79,19,-8,[#####] Excellent

```

---

## ⚠️ Troubleshooting

**1. "Connection Failed"**

* Check if the USB cable is plugged in.
* Verify the port name: `ls /dev/ttyUSB*`.
* Ensure you are running with `sudo`.

**2. Data stops updating**

* This tool is designed to auto-reconnect. If it stops for >10 seconds, the modem itself may have rebooted or lost power.

**3. "Permission Denied"**

* You must run the tool as root: `sudo ./advanced_monitor`.
