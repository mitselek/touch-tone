#include <Arduino.h>

// Define the Serial port pins for SIM800L
#define RXD2 16
#define TXD2 17

// Define the Serial port instance for SIM800L
#define SIM800_SERIAL Serial2

// Function declaration
void processSmsNotification(String notificationLine);
void processIncomingCall(String notificationLine);

void setup() {
  // Start Serial monitor communication
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("ESP32 Serial Monitor Started");

  // Start Hardware Serial communication with SIM800L
  // Common baud rates for SIM800L are 9600, 19200, 38400, 57600, 115200
  // Start with 9600 or check your module's default. Auto-bauding can be complex.
  SIM800_SERIAL.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Initializing SIM800L...");

  // Give the modem some time to power up and register
  delay(3000);

  // Send a basic AT command to check connectivity
  SIM800_SERIAL.println("AT");
  delay(100); // Wait a bit for response

  // Disable command echo
  SIM800_SERIAL.println("ATE0");
  delay(100); // Wait a bit for response

  // Set SMS mode to Text Mode
  SIM800_SERIAL.println("AT+CMGF=1");
  delay(100); // Wait a bit for response

  // Optionally, you might want to configure SMS notifications:
  // AT+CNMI=2,1,0,0,0  (Route SMS notifications to UART, e.g., +CMTI: "SM", index)
  SIM800_SERIAL.println("AT+CNMI=2,1,0,0,0");
  delay(100);

  Serial.println("SIM800L Initialized (check Serial Monitor for responses).");
  Serial.println("Ready to forward data between Serial Monitor and SIM800L.");
  Serial.println("Type AT commands here to send to SIM800L.");
  Serial.println("--------------------------------------------------");

}

// Function definition to process SMS notifications
void processSmsNotification(String notificationLine) {
    Serial.println("\n*** New SMS Notification Received! Attempting to read... ***");
    // Example line: +CMTI: "SM",14
    int commaIndex = notificationLine.lastIndexOf(',');
    if (commaIndex != -1) {
        String indexStr = notificationLine.substring(commaIndex + 1);
        indexStr.trim(); // Trim whitespace just in case
        int messageIndex = indexStr.toInt();
        if (messageIndex > 0) {
            Serial.print("Reading SMS at index: ");
            Serial.println(messageIndex);
            SIM800_SERIAL.print("AT+CMGR="); // Send command to read SMS at the specified index
            SIM800_SERIAL.println(messageIndex);
            // The response will be printed in the next loop iteration(s)

            // Add a small delay before deleting, allowing time for read command processing if needed
            delay(200); 

            Serial.print("Deleting SMS at index: ");
            Serial.println(messageIndex);
            SIM800_SERIAL.print("AT+CMGD="); // Send command to delete SMS at the specified index
            SIM800_SERIAL.println(messageIndex);
            // The response (usually "OK") will be printed in the next loop iteration(s)

        } else {
            Serial.println("Could not parse SMS index.");
        }
    } else {
        Serial.println("Could not find comma in +CMTI line to parse index.");
    }
    Serial.println("--------------------------------------------------");
}

// Function definition to process incoming call notifications
void processIncomingCall(String notificationLine) {
    Serial.println("\n*** Incoming Call Notification Received! ***");
    Serial.println("Type 'ATA' to answer or 'ATH' to hang up.");
    Serial.println("--------------------------------------------------");
}

void loop() {
  // Forward data from SIM800L to Serial Monitor, reading line by line
  if (SIM800_SERIAL.available()) {
    String line = SIM800_SERIAL.readStringUntil('\n');
    line.trim(); // Remove potential leading/trailing whitespace/CR
    if (line.length() > 0) { // Only print if the line is not empty
        Serial.print("SIM800L: ");
        Serial.println(line); // Print the received line

        // Check for new SMS notification
        if (line.startsWith("+CMTI:")) {
            processSmsNotification(line); // Call the dedicated function
        }

        // Check for incoming call notification
        if (line.startsWith("RING")) {
            processIncomingCall(line); // Call the dedicated function
        }
    }
  }

  // Forward data from Serial Monitor to SIM800L
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove potential leading/trailing whitespace/CR
    if (command.equalsIgnoreCase("ATA")) {
        SIM800_SERIAL.println("ATA"); // Answer the call
        Serial.println("Answering the call...");
    } else if (command.equalsIgnoreCase("ATH")) {
        SIM800_SERIAL.println("ATH"); // Hang up the call
        Serial.println("Hanging up the call...");
    } else {
        SIM800_SERIAL.println(command); // Forward other commands to SIM800L
        Serial.println(command); // Echo the command back to the Serial Monitor
    }
  }
}
