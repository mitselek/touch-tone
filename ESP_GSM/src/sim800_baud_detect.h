// SIM800L baud rate detection utility
// This function can be included in main.cpp or other files as needed
#include <Arduino.h>

extern HardwareSerial SIM800_SERIAL;

// Try a list of baud rates (fastest to slowest) and return the working one, or -1 if none work
int autoDetectSIM800Baud(const int* baudRates, int numRates, int rxPin, int txPin, bool verbose = true) {
    for (int i = 0; i < numRates; ++i) {
        int baud = baudRates[i];
        SIM800_SERIAL.end();
        delay(100);
        SIM800_SERIAL.begin(baud, SERIAL_8N1, rxPin, txPin);
        if (verbose) {
            Serial.print("Testing SIM800L at baud: ");
            Serial.println(baud);
        }
        delay(1000); // Give SIM800L a moment to adjust
        SIM800_SERIAL.println("AT");
        unsigned long start = millis();
        String response = "";
        while (millis() - start < 2000) {
            while (SIM800_SERIAL.available()) {
                char c = SIM800_SERIAL.read();
                response += c;
            }
            yield();
        }
        if (response.indexOf("OK") != -1) {
            if (verbose) {
                Serial.print("SUCCESS: SIM800L responded at baud ");
                Serial.println(baud);
            }
            return baud;
        } else {
            if (verbose) {
                Serial.print("No response at baud ");
                Serial.println(baud);
            }
        }
    }
    return -1;
}
