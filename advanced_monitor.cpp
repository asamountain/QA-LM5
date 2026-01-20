#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <ctime>
#include <csignal>

// ==========================================
// SETTINGS
// ==========================================
const char* PORT_NAME = "/dev/ttyUSB7"; 
const int BAUD_RATE = B115200;
const char* LOG_FILE_NAME = "lte_signal_log.csv";
// ==========================================

using namespace std;

string get_timestamp() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    return string(buf);
}

const char* get_quality_bar(int rsrp) {
    if (rsrp == 0 || rsrp < -140) return "[!] NO SIGNAL "; 
    if (rsrp > -80)  return "[#####] Excellent";
    if (rsrp > -90)  return "[#### ] Good     ";
    if (rsrp > -105) return "[###  ] Fair     ";
    if (rsrp > -115) return "[##   ] Poor     ";
    return "[#    ] Critical ";
}

int setup_serial(const char* portname) {
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) return -1;

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_iflag = 0;
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0; 

    tcsetattr(fd, TCSANOW, &tty);
    return fd;
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    setbuf(stdout, NULL); 

    ofstream logFile;
    logFile.open(LOG_FILE_NAME, ios::app); 
    
    // Header updated with RSRQ
    if (logFile.tellp() == 0) {
        logFile << "Timestamp,RSSI,RSRP,SINR,RSRQ,Quality_Status" << endl;
    }

    printf("LM5 Full QA Monitor (RSSI + RSRP + SINR + RSRQ)\n");
    printf("Port: %s | Log: %s\n", PORT_NAME, LOG_FILE_NAME);
    printf("---------------------------------------------------------------------------------------\n");
    printf("TIMESTAMP           | RSSI  | RSRP (True Signal)    | SINR | RSRQ  | VISUAL CHART\n");
    printf("---------------------------------------------------------------------------------------\n");
    
    int fd = -1;

    while (true) {
        if (fd < 0) {
            fd = setup_serial(PORT_NAME);
            if (fd < 0) {
                printf(".\r"); 
                fflush(stdout);
                this_thread::sleep_for(chrono::seconds(1));
                continue;
            } else {
                printf("\n[INFO] Connected to Modem!\n");
            }
        }

        string command = "AT+QCSQ\r\n"; 
        tcflush(fd, TCIOFLUSH); 
        int write_status = write(fd, command.c_str(), command.length());
        
        if (write_status < 0) {
            printf("\n[ERROR] USB Disconnected. Retrying...\n");
            close(fd);
            fd = -1; 
            continue;
        }

        char buf[1]; 
        string line_buffer = "";
        bool data_found = false;
        
        for (int i = 0; i < 100; i++) { 
            int n = read(fd, buf, 1);
            if (n > 0) {
                line_buffer += buf[0];
                if (buf[0] == '\n') {
                    if (line_buffer.find("+QCSQ:") != string::npos) {
                        data_found = true;
                        break; 
                    }
                    line_buffer = ""; 
                }
            } else {
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

        if (data_found) {
            size_t pos = line_buffer.find("+QCSQ:");
            try {
                size_t start = line_buffer.find(",", pos); 
                if (start != string::npos) {
                    stringstream ss(line_buffer.substr(start + 1));
                    string segment;
                    vector<int> values;

                    while(getline(ss, segment, ',')) {
                        try { values.push_back(stoi(segment)); } catch(...) { values.push_back(-999); }
                    }

                    // We need at least 4 values now (RSSI, RSRP, SINR, RSRQ)
                    if (values.size() >= 4) {
                        int rssi = values[0];
                        int rsrp = values[1]; 
                        int sinr = values[2] / 10; // SINR is often scaled
                        int rsrq = values[3];      // RSRQ is usually raw dB value
                        
                        string ts = get_timestamp();
                        const char* bar = get_quality_bar(rsrp);

                        // Updated Print Format
                        printf("%s | %d | %d dBm %s | %d dB | %d | \n", 
                               ts.c_str(), rssi, rsrp, bar, sinr, rsrq);
                        
                        if (logFile.is_open()) {
                            logFile << ts << "," << rssi << "," << rsrp << "," << sinr << "," << rsrq << "," << bar << endl;
                            logFile.flush(); 
                        }
                    }
                }
            } catch (...) {}
        }
        
        this_thread::sleep_for(chrono::seconds(1));
    }

    if (fd >= 0) close(fd);
    logFile.close();
    return 0;
}
