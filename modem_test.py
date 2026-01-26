import serial
import time
import sys

# ==========================================
# SETTINGS
# ==========================================
# Change this to your port (/dev/ttyUSB2 or /dev/ttyUSB7)
PORT = '/dev/ttyUSB2'  
BAUD_RATE = 115200
# ==========================================

def start_terminal():
    print(f"🔌 Connecting to Modem on {PORT}...")
    
    try:
        # Open the Serial Port
        ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
        print("✅ Connected! Type your AT command and press ENTER.")
        print("   (Type 'exit' or 'quit' to stop)\n")

        while True:
            # 1. Get User Input
            user_input = input(">> Enter Command: ")

            # Check if user wants to quit
            if user_input.lower() in ['exit', 'quit']:
                print("👋 Exiting...")
                break
            
            # 2. Send Command to Modem (Add \r\n automatically)
            full_command = user_input + "\r\n"
            ser.write(full_command.encode())

            # 3. Wait a bit for modem to think
            time.sleep(0.5) 

            # 4. Read Response
            if ser.in_waiting > 0:
                response = ser.read_all().decode(errors='ignore').strip()
                print(f"📝 Response:\n{response}\n")
            else:
                print("⚠️ No Response (Modem is silent)\n")

        ser.close()

    except serial.SerialException as e:
        print(f"❌ Error: Could not open port {PORT}.")
        print(f"   Details: {e}")
    except KeyboardInterrupt:
        print("\n👋 Force Quit detected. Closing connection.")
        if 'ser' in locals() and ser.is_open:
            ser.close()

if __name__ == "__main__":
    start_terminal()
