#include <Arduino.h>
// #include "sim800_baud_detect.h"

// Define the Serial port pins for SIM800L
#define RXD2 16
#define TXD2 17

// Define the Serial port instance for SIM800L
#define SIM800_SERIAL Serial2

// Function declaration
void processSmsNotification(String notificationLine);
void processIncomingCall(String notificationLine);
void sendCommandToSIM800L(const String& command);
void printWithTime(const String& message);
bool sendCommandAndWait(const String& command, const String& expectedResponse, unsigned long timeout);
void sendSms(const String& phoneNumber, const String& message);
void processSmsContent(String line);

// Global variable to track the last "Call Ready" timestamp
unsigned long lastCallReadyTime = 0;

// Global variables for SMS processing
String smsPhoneNumber = "";
String smsMessageContent = "";
bool waitingForSmsHeader = false;
bool waitingForSmsContent = false;
int currentSmsIndex = -1;

// SMS state machine
enum SmsState {
    SMS_IDLE,
    SMS_WAIT_HEADER,
    SMS_WAIT_CONTENT
};
SmsState smsState = SMS_IDLE;
int smsIndexToDelete = -1;

// Helper function to prepend time to Serial output
void printWithTime(const String& message) {
    Serial.print("[");
    Serial.print(millis());
    Serial.print(" ms] ");
    Serial.println(message);
}

void sendCommandToSIM800L(const String& command) {
    SIM800_SERIAL.println(command); // Send the command to SIM800L
    printWithTime("Command to SIM800L: " + command); // Print the command with time
}

// Helper function to send a command and wait for a response
bool sendCommandAndWait(const String& command, const String& expectedResponse, unsigned long timeout = 5000) {
    sendCommandToSIM800L(command); // Send the command
    unsigned long startTime = millis();
    String currentLine = "";
    char receivedChar;
    // printWithTime("Waiting for response to: " + command + " (timeout: " + String(timeout) + "ms)"); // Optional: uncomment for more verbose logging

    while (millis() - startTime < timeout) {
        while (SIM800_SERIAL.available()) {
            receivedChar = SIM800_SERIAL.read();
            // Serial.write(receivedChar); // Optional: Echo raw char to main serial for extreme debugging

            if (receivedChar == '\n') {
                currentLine.trim(); // Trim whitespace from the completed line
                if (currentLine.length() > 0) {
                    printWithTime("SIM800L Raw Line: " + currentLine);
                    if (currentLine.indexOf(expectedResponse) != -1) {
                        printWithTime("SIM800L: + Expected response found in: " + currentLine);
                        return true; // Expected response received
                    } else {
                        // Only print if it's not an empty line or just "AT" echoed back if echo is on
                        if (!currentLine.equalsIgnoreCase(command) && !currentLine.isEmpty()) {
                           printWithTime("SIM800L: - Unexpected line: " + currentLine);
                        }
                    }
                }
                currentLine = ""; // Reset for the next line
            } else if (receivedChar != '\r') { // Ignore carriage return, accumulate others
                currentLine += receivedChar;
            }
        }
        yield(); // Allow other tasks to run, important for ESP32
    }

    // Check if there's any partial line data at timeout (e.g. if SIM800L didn't send \n)
    currentLine.trim();
    if (currentLine.length() > 0) {
        printWithTime("SIM800L Partial Line at Timeout: " + currentLine);
        if (currentLine.indexOf(expectedResponse) != -1) {
            printWithTime("SIM800L: + Expected response found in partial line: " + currentLine);
            return true;
        } else {
            if (!currentLine.equalsIgnoreCase(command) && !currentLine.isEmpty()) {
                printWithTime("SIM800L: - Unexpected partial line at timeout: " + currentLine);
            }
        }
    }
    
    printWithTime("Timeout waiting for '" + expectedResponse + "' in response to: " + command);
    return false; // Timeout or unexpected response
}

// Try a list of baud rates (fastest to slowest) and return the working one, or -1 if none work
int autoDetectSIM800Baud(const int* baudRates, int numRates, int rxPin, int txPin) {
    for (int i = 0; i < numRates; ++i) {
        int baud = baudRates[i];
        SIM800_SERIAL.end();
        delay(100);
        SIM800_SERIAL.begin(baud, SERIAL_8N1, rxPin, txPin);
        printWithTime("Testing SIM800L at baud: " + String(baud));
        delay(3000); // Give SIM800L a moment to adjust
        if (sendCommandAndWait("AT", "OK", 2000)) {
            printWithTime("SUCCESS: SIM800L responded at baud " + String(baud));
            return baud;
        } else {
            printWithTime("No response at baud " + String(baud));
        }
    }
    return -1;
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    printWithTime("Serial Monitor Initialized at 115200 baud.");

    SIM800_SERIAL.begin(57600, SERIAL_8N1, RXD2, TXD2);
    printWithTime("SIM800L Serial Initialized at 57600 baud on pins RX: " + String(RXD2) + ", TX: " + String(TXD2));
    delay(2000);
    printWithTime("Waiting for SIM800L to settle (5 seconds)...");
    delay(5000);
    printWithTime("Sending initial AT commands to SIM800L...");

    // Basic AT command to check communication
    if (sendCommandAndWait("AT", "OK", 7000)) { // Explicit 7s timeout, longer to accommodate potential slow init
        printWithTime("SIM800L is responsive to AT command.");
    } else {
        printWithTime("SIM800L did not respond as expected to AT command. Dumping raw serial from SIM800L for 5 seconds...");
        unsigned long debugStartTime = millis();
        bool receivedData = false;
        while(millis() - debugStartTime < 5000) { // Listen for 5 seconds
            if (SIM800_SERIAL.available()) {
                Serial.write(SIM800_SERIAL.read()); // Print raw data to main serial
                receivedData = true;
            }
            yield(); // Allow other ESP32 tasks to run
        }
        if (receivedData) {
            Serial.println("\\nRaw data dump complete.");
        } else {
            Serial.println("\\nNo raw data received from SIM800L during dump.");
        }
        printWithTime("Halting due to SIM800L communication failure at initial AT command.");
        while(1) {
            yield(); // Halt if basic communication fails, but keep yielding
        }
    }

    // Disable command echo
    if (sendCommandAndWait("ATE0", "OK", 2000)) { // Shorter timeout for ATE0
        printWithTime("Command echo disabled (ATE0).");
    } else {
        printWithTime("Failed to disable command echo (ATE0). This might be okay if it's already off or module doesn't support ATE0 well. Continuing...");
        // It's not critical to halt here, but responses might include the command itself.
    }

    // Retry each command up to three times
    for (int attempt = 0; attempt < 3; ++attempt) {
        if (sendCommandAndWait("AT", "OK")) break;
        printWithTime("Retrying: Failed to communicate with SIM800L. Check connections.");
        delay(1000);
    }

    for (int attempt = 0; attempt < 3; ++attempt) {
        if (sendCommandAndWait("ATE0", "OK")) break;
        printWithTime("Retrying: Failed to disable command echo.");
        delay(1000);
    }

    for (int attempt = 0; attempt < 3; ++attempt) {
        if (sendCommandAndWait("AT+CMGF=1", "OK")) break;
        printWithTime("Retrying: Failed to set SMS mode to Text Mode.");
        delay(1000);
    }

    for (int attempt = 0; ++attempt < 3; ++attempt) {
        if (sendCommandAndWait("AT+CNMI=2,1,0,0,0", "OK")) break;
        printWithTime("Retrying: Failed to configure SMS notifications.");
        delay(1000);
    }

    for (int attempt = 0; ++attempt < 3; ++attempt) {
        if (sendCommandAndWait("AT+CSMINS?", "+CSMINS:")) break;
        printWithTime("Retrying: Failed to check SIM card presence.");
        delay(1000);
    }

    for (int attempt = 0; ++attempt < 3; ++attempt) {
        if (sendCommandAndWait("AT+CPIN?", "+CPIN: READY")) break;
        printWithTime("Retrying: SIM card is not ready. Check if PIN is required.");
        delay(1000);
    }

    for (int attempt = 0; ++attempt < 3; ++attempt) {
        if (sendCommandAndWait("AT+CREG?", "+CREG:")) break;
        printWithTime("Retrying: Failed to check network registration.");
        delay(1000);
    }

    for (int attempt = 0; ++attempt < 3; ++attempt) {
        if (sendCommandAndWait("AT+CSQ", "+CSQ:")) break;
        printWithTime("Retrying: Failed to check signal quality.");
        delay(1000);
    }

    // Check SIM800L firmware version and module information
    printWithTime("Checking SIM800L firmware information...");
    sendCommandAndWait("ATI", "OK"); // Get module information
    delay(500);
    sendCommandAndWait("AT+CGMR", "OK"); // Get firmware version
    delay(500);
    printWithTime("SIM800L Initialized successfully.");
    printWithTime("Ready to forward data between Serial Monitor and SIM800L.");
    printWithTime("Type AT commands here to send to SIM800L.");
    printWithTime("--------------------------------------------------");
}

// New function to send SMS
void sendSms(const String& phoneNumber, const String& message) {
    printWithTime("Sending SMS to: " + phoneNumber);
    printWithTime("Message: " + message);
    
    sendCommandToSIM800L("AT+CMGS=\"" + phoneNumber + "\""); // Command to send SMS
    delay(100);
    
    // Send message content followed by Ctrl+Z (ASCII 26) to end message
    SIM800_SERIAL.print(message);
    SIM800_SERIAL.write(26);
    
    printWithTime("SMS sent.");
}

// Function definition to process SMS notifications
void processSmsNotification(String notificationLine) {
    printWithTime("*** New SMS Notification Received! Attempting to read... ***");
    // Example: +CMTI: "SM",3
    int commaIndex = notificationLine.lastIndexOf(',');
    if (commaIndex != -1) {
        String idxStr = notificationLine.substring(commaIndex + 1);
        idxStr.trim();
        int idx = idxStr.toInt();
        if (idx > 0) {
            smsPhoneNumber = "";
            smsMessageContent = "";
            smsIndexToDelete = idx;
            smsState = SMS_WAIT_HEADER;
            printWithTime("Reading SMS at index: " + String(idx));
            sendCommandToSIM800L("AT+CMGR=" + String(idx));
        } else {
            printWithTime("Could not parse SMS index.");
        }
    } else {
        printWithTime("Could not find comma in +CMTI line to parse index.");
    }
}

void handleSmsLine(const String& line) {
    switch (smsState) {
        case SMS_WAIT_HEADER:
            printWithTime("Waiting for SMS header... " + line);
            if (line.startsWith("+CMGR:")) {
                // Format: +CMGR: "REC UNREAD","+37256560978","","25/04/23,02:00:30+12"
                int firstQuote = line.indexOf("\",\"") + 3;
                int secondQuote = line.indexOf("\"", firstQuote);
                printWithTime("Quotes found at: " + String(firstQuote) + ", " + String(secondQuote));
                if (firstQuote > 1 && secondQuote > firstQuote) {
                    smsPhoneNumber = line.substring(firstQuote, secondQuote);
                    printWithTime("SMS from: " + smsPhoneNumber);
                    smsState = SMS_WAIT_CONTENT;
                }
            }
            break;
        case SMS_WAIT_CONTENT:
            if (line.equals("OK")) {
                if (smsPhoneNumber.length() > 0 && smsMessageContent.length() > 0) {
                    // Reply
                    String reply = smsMessageContent + " Sulle ka! :)";
                    printWithTime("Replying to: " + smsPhoneNumber + " with: " + reply);
                    sendCommandToSIM800L("AT+CMGS=\"" + smsPhoneNumber + "\"");
                    delay(300);
                    SIM800_SERIAL.print(reply);
                    SIM800_SERIAL.write(26); // Ctrl+Z
                    delay(5000); // Wait for SMS to send

                    // Delete original SMS
                    if (smsIndexToDelete > 0) {
                        printWithTime("Deleting SMS at index: " + String(smsIndexToDelete));
                        sendCommandToSIM800L("AT+CMGD=" + String(smsIndexToDelete));
                    }
                }
                // Reset state
                smsState = SMS_IDLE;
                smsPhoneNumber = "";
                smsMessageContent = "";
                smsIndexToDelete = -1;
            } else if (smsMessageContent.isEmpty()) {
                smsMessageContent = line;
                printWithTime("SMS content: " + smsMessageContent);
            }
            break;
        default:
            break;
    }
}

void processIncomingCall(String notificationLine) {
    printWithTime("*** Incoming Call Notification Received! ***");

    // Extract and print the caller's number
    int startIndex = notificationLine.indexOf("\"") + 1;
    int endIndex = notificationLine.indexOf("\"", startIndex);
    if (startIndex > 0 && endIndex > startIndex) {
        String callerNumber = notificationLine.substring(startIndex, endIndex);
        printWithTime("Caller Number: " + callerNumber);
    } else {
        printWithTime("Failed to extract caller number.");
    }

    printWithTime("Type 'ATA' to answer or 'ATH' to hang up.");
    printWithTime("--------------------------------------------------");
}

void loop() {
  // Forward data from SIM800L to Serial Monitor, reading line by line
  if (SIM800_SERIAL.available()) {
    String line = SIM800_SERIAL.readStringUntil('\n');
    line.trim(); // Remove potential leading/trailing whitespace/CR
    if (line.length() > 0) { // Only print if the line is not empty
        printWithTime("tic: " + line); // Print the received line with time
        
        // SMS state machine
        if (smsState == SMS_WAIT_HEADER || smsState == SMS_WAIT_CONTENT) {
            handleSmsLine(line);
        }

        // Handle "NO CARRIER" message
        if (line.equalsIgnoreCase("NO CARRIER")) {
            printWithTime("Call disconnected. Stopping DTMF transmission.");
            return; // Exit early to stop further processing
        }

        // Check for "Call Ready" message
        if (line.equalsIgnoreCase("Call Ready")) {
            unsigned long currentTime = millis();
            if (lastCallReadyTime > 0) {
                unsigned long interval = currentTime - lastCallReadyTime;
                printWithTime("Interval since last 'Call Ready': " + String(interval) + " ms");
            }
            lastCallReadyTime = currentTime;
        }

        // Check for new SMS notification
        if (line.startsWith("+CMTI:")) {
            processSmsNotification(line); // Call the dedicated function
        }

        // Check for incoming call notification
        if (line.startsWith("RING")) {
            processIncomingCall(line); // Call the dedicated function
        }

        // Handle network registration status
        if (line.startsWith("+CREG:")) {
            int status = line.charAt(line.lastIndexOf(',') + 1) - '0';
            if (status == 1 || status == 5) {
                printWithTime("SIM800L is registered on the network.");
            } else if (status == 2) {
                printWithTime("SIM800L is searching for a network...");
            } else {
                printWithTime("SIM800L is not registered on the network.");
            }
        }
    }
  }

  // Forward data from Serial Monitor to SIM800L
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove potential leading/trailing whitespace/CR
    sendCommandToSIM800L(command); // Forward command to SIM800L and print
    if (command.equalsIgnoreCase("ATA")) {
        printWithTime("Answering the call...");
        delay(1000); // Wait for the call to connect
    } else if (command.equalsIgnoreCase("ATH")) {
        printWithTime("Hanging up the call...");
    } else {
        printWithTime(command); // Echo the command back to the Serial Monitor
    }
  }

  // Check network registration periodically
  static unsigned long lastNetworkCheck = 0;
  static bool isRegistered = false; // Track registration status
  if (millis() - lastNetworkCheck > 60e3) { // Every 1 minute
    if (!isRegistered) {
      sendCommandToSIM800L("AT+CREG?"); // Check network registration
    }
    lastNetworkCheck = millis();
  }
}
