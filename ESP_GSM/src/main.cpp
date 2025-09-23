#include <Arduino.h>
#include <Preferences.h>

/*
 * Phone Zero - ESP32 Version
 * Speed dial system for elderly/blind users
 * Supports 8 tactile buttons for instant calling
 * SMS programming and authorized number whitelist
 * 
 * Hardware:
 * - ESP32-WROOM-32 (DoiT ESP32 DevKit v1)
 * - SIM900A GSM module (independent 5V/2A power)
 * - 8 tactile buttons with pull-up resistors
 * 
 * Connections:
 * - SIM900A TX -> ESP32 D16 (GPIO16, UART2 RX)
 * - SIM900A RX -> ESP32 D17 (GPIO17, UART2 TX)
 * - SIM900A GND -> ESP32 GND (shared ground)
 * - SIM900A VCC -> External 5V/2A supply
 * 
 * Button Pins (with internal pull-ups):
 * - Button 1: GPIO4  (avoid strapping pins when possible)
 * - Button 2: GPIO18 
 * - Button 3: GPIO19
 * - Button 4: GPIO21 
 * - Button 5: GPIO22
 * - Button 6: GPIO23
 * - Button 7: GPIO25
 * - Button 8: GPIO26
 */

// SIM900A Hardware Serial (UART2)
HardwareSerial sim900(2); // UART2
#define SIM900_RX_PIN 16   // ESP32 D16 -> SIM900A TX
#define SIM900_TX_PIN 17   // ESP32 D17 -> SIM900A RX

// Button pins for ESP32 (avoiding strapping pins where possible)
#define BUTTON_1 4
#define BUTTON_2 18
#define BUTTON_3 19
#define BUTTON_4 21
#define BUTTON_5 22
#define BUTTON_6 23
#define BUTTON_7 25
#define BUTTON_8 26

// Speed dial numbers (stored in Flash memory via Preferences)
String speedDialNumbers[8] = {
  "+37256560978",  // Button 1 - default number
  "+37255639121",  // Button 2 - default number
  "",              // Button 3
  "",              // Button 4
  "",              // Button 5
  "",              // Button 6
  "",              // Button 7
  ""               // Button 8
};

// Authorized numbers for SMS programming
String authorizedNumbers[] = {
  "+37256560978",
  "+37255639121"
};
const int numAuthorizedNumbers = 2;

// Button handling
int buttonPins[8] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8};
bool lastButtonState[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTime[8] = {0, 0, 0, 0, 0, 0, 0, 0};
const unsigned long debounceDelay = 50;

bool incomingCall = false;
Preferences preferences;

// Function declarations
void checkButtons();
void handleButtonPress(int buttonNumber);
void makeCall(String number);
void handleIncomingCall();
void handleIncomingSMS(String response);
void processSMSCommand(String command, String sender);
void sendSMS(String number, String message);
void deleteAllSMS();
void checkSMSMemory();
void initSIM900();
void readSIM900Response(String command);
void loadSpeedDialNumbers();
void saveSpeedDialNumbers();
void printSpeedDialNumbers();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Phone Zero - ESP32 Version Starting...");
  Serial.println("=======================================");
  Serial.println("Using Hardware UART2 (D16/D17) for SIM900A");
  Serial.println("SIM900A powered independently with 5V/2A");
  
  // Initialize UART2 for SIM900A communication
  sim900.begin(9600, SERIAL_8N1, SIM900_RX_PIN, SIM900_TX_PIN);
  Serial.println("ESP32 UART2 initialized at 9600 baud");
  
  // Initialize button pins with internal pull-ups
  for (int i = 0; i < 8; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    Serial.print("Button ");
    Serial.print(i + 1);
    Serial.print(" on GPIO");
    Serial.println(buttonPins[i]);
  }
  
  // Initialize Preferences for persistent storage
  preferences.begin("phone-zero", false);
  
  // Load speed dial numbers from flash memory
  loadSpeedDialNumbers();
  
  // Set default number for button 1 if not already programmed
  if (speedDialNumbers[0].length() == 0) {
    speedDialNumbers[0] = "+37256560978";
    saveSpeedDialNumbers();
    Serial.println("Set default number for Button 1: +37256560978");
  }
  
  // Give SIM900A time to be ready
  Serial.println("Waiting for SIM900A to boot...");
  delay(3000);

  // Initialize SIM900A
  initSIM900();
  
  Serial.println("\nPhone Zero ESP32 ready!");
  Serial.println("Press buttons 1-8 for speed dial");
  Serial.println("Send SMS to program numbers: 'SET 1 +1234567890'");
  printSpeedDialNumbers();
}

void loop() {
  static unsigned long lastSMSCheck = 0;
  
  // Check for button presses
  checkButtons();
  
  // Check for incoming calls/SMS from SIM900A
  if (sim900.available()) {
    String response = sim900.readString();
    Serial.print("SIM900A: ");
    Serial.println(response);
    
    if (response.indexOf("RING") != -1) {
      handleIncomingCall();
    } else if (response.indexOf("+CMT:") != -1) {
      handleIncomingSMS(response);
    }
  }
  
  // Check for serial input (AT commands from computer)
  if (Serial.available()) {
    String command = Serial.readString();
    command.trim(); // Remove whitespace
    
    Serial.print("Received command: ");
    Serial.println(command);
    
    // Handle special commands
    if (command.equalsIgnoreCase("SMSCHECK")) {
      checkSMSMemory();
    } else if (command.equalsIgnoreCase("SMSCLEAR")) {
      deleteAllSMS();
    } else {
      // Forward to SIM900A
      sim900.println(command);
      
      // Wait a bit for response
      delay(100);
      if (sim900.available()) {
        String response = sim900.readString();
        Serial.print("AT Response: ");
        Serial.println(response);
      }
    }
  }
  
  // Periodic SMS memory cleanup (every 10 minutes)
  if (millis() - lastSMSCheck > 600000) {
    Serial.println("Periodic SMS memory cleanup...");
    deleteAllSMS();
    lastSMSCheck = millis();
  }
  
  delay(50); // Shorter delay for more responsive buttons
}

void checkButtons() {
  static unsigned long lastDebugTime = 0;
  
  for (int i = 0; i < 8; i++) {
    bool reading = digitalRead(buttonPins[i]);
    
    // No more debug output for GPIO4 - removed to reduce noise
    
    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
      Serial.print("Button ");
      Serial.print(i + 1);
      Serial.print(" state changed to ");
      Serial.println(reading ? "HIGH" : "LOW");
      lastButtonState[i] = reading;
    }
    
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading == LOW && lastButtonState[i] == LOW) {
        // Button is pressed and state is stable
        Serial.print("Button ");
        Serial.print(i + 1);
        Serial.println(" PRESSED! Triggering call...");
        handleButtonPress(i + 1);
        
        // Wait for button release to avoid multiple triggers
        while (digitalRead(buttonPins[i]) == LOW) {
          delay(10);
        }
        lastButtonState[i] = HIGH;
        lastDebounceTime[i] = millis();
      }
    }
  }
}

void handleButtonPress(int buttonNumber) {
  Serial.print("Button ");
  Serial.print(buttonNumber);
  Serial.print(" pressed (GPIO");
  Serial.print(buttonPins[buttonNumber - 1]);
  Serial.println(")");
  
  String number = speedDialNumbers[buttonNumber - 1];
  
  if (number.length() > 0 && number != "") {
    makeCall(number);
  } else {
    Serial.println("No number programmed for this button");
    Serial.println("Send SMS: 'SET " + String(buttonNumber) + " +1234567890'");
  }
}

void makeCall(String number) {
  Serial.print("Calling: ");
  Serial.println(number);
  
  sim900.print("ATD");
  sim900.print(number);
  sim900.println(";"); // Semicolon indicates voice call
  
  delay(1000);
  
  // Read response
  if (sim900.available()) {
    String response = sim900.readString();
    Serial.print("Call response: ");
    Serial.println(response);
  }
}

void handleIncomingCall() {
  Serial.println("*** INCOMING CALL DETECTED ***");
  incomingCall = true;
  
  // Auto-answer after 3 rings (optional)
  delay(3000);
  sim900.println("ATA"); // Answer call
  Serial.println("Call answered automatically");
}

void handleIncomingSMS(String response) {
  Serial.println("*** SMS RECEIVED - PROCESSING ***");
  Serial.print("Raw SMS data: ");
  Serial.println(response);
  
  // Extract phone number and message content
  int phoneStart = response.indexOf("+37");
  if (phoneStart == -1) {
    Serial.println("Could not find phone number");
    // Clean up any SMS in memory
    deleteAllSMS();
    return;
  }
  
  int phoneEnd = response.indexOf("\"", phoneStart);
  if (phoneEnd == -1) {
    Serial.println("Could not parse phone number");
    // Clean up any SMS in memory
    deleteAllSMS();
    return;
  }
  
  String senderNumber = response.substring(phoneStart, phoneEnd);
  Serial.print("SMS from: ");
  Serial.println(senderNumber);
  
  // Check if sender is authorized
  bool authorized = false;
  for (int i = 0; i < numAuthorizedNumbers; i++) {
    if (senderNumber == authorizedNumbers[i]) {
      authorized = true;
      break;
    }
  }
  
  if (!authorized) {
    Serial.println("*** UNAUTHORIZED SMS SENDER - IGNORED ***");
    // Clean up any SMS in memory
    deleteAllSMS();
    return;
  }
  
  Serial.println("Sender authorized - processing command");
  
  // Simple and robust message extraction
  // Split by lines and find the message (not the +CMT header)
  String lines[10]; // Support up to 10 lines
  int lineCount = 0;
  int startPos = 0;
  
  // Split response into lines
  while (startPos < response.length() && lineCount < 10) {
    int endPos = response.indexOf('\n', startPos);
    if (endPos == -1) {
      lines[lineCount] = response.substring(startPos);
      lineCount++;
      break;
    } else {
      lines[lineCount] = response.substring(startPos, endPos);
      lineCount++;
      startPos = endPos + 1;
    }
  }
  
  // Debug: show all lines
  Serial.println("Lines found:");
  for (int i = 0; i < lineCount; i++) {
    Serial.print("Line ");
    Serial.print(i);
    Serial.print(": '");
    Serial.print(lines[i]);
    Serial.println("'");
  }
  
  // Find the message line (not the +CMT header)
  String message = "";
  for (int i = 0; i < lineCount; i++) {
    String line = lines[i];
    line.trim();
    
    // Skip empty lines, header lines, and lines with only non-printable characters
    if (line.length() > 0 && !line.startsWith("+CMT:")) {
      // Check if the line contains mostly printable characters
      int printableCount = 0;
      for (int j = 0; j < line.length(); j++) {
        char c = line.charAt(j);
        if (c >= 32 && c <= 126) { // Printable ASCII range
          printableCount++;
        }
      }
      
      // If more than 50% of characters are printable, consider it valid
      if (printableCount > line.length() / 2) {
        message = line;
        Serial.print("Selected line ");
        Serial.print(i);
        Serial.println(" as message content");
        break; // Take the first valid non-header line
      }
    }
  }
  
  // Clean the message
  message.trim();
  
  // Remove any non-printable characters at the end
  while (message.length() > 0 && message.charAt(message.length() - 1) < 32) {
    message = message.substring(0, message.length() - 1);
  }
  
  Serial.print("SMS command: '");
  Serial.print(message);
  Serial.println("'");
  
  if (message.length() == 0) {
    Serial.println("Empty message content!");
    deleteAllSMS();
    return;
  }
  
  processSMSCommand(message, senderNumber);
  
  // Clean up SMS memory after processing
  deleteAllSMS();
}

void processSMSCommand(String command, String sender) {
  // Don't convert to uppercase for SMS command to preserve message text
  String upperCommand = command;
  upperCommand.toUpperCase();
  
  if (upperCommand.startsWith("SET")) {
    // Format: "SET 1 +37256560978"
    int spacePos = upperCommand.indexOf(' ', 4);
    if (spacePos == -1) {
      sendSMS(sender, "Invalid format. Use: SET 1 +1234567890");
      return;
    }
    
    int buttonNum = upperCommand.substring(4, spacePos).toInt();
    String phoneNumber = upperCommand.substring(spacePos + 1);
    phoneNumber.trim();
    
    if (buttonNum >= 1 && buttonNum <= 8) {
      speedDialNumbers[buttonNum - 1] = phoneNumber;
      saveSpeedDialNumbers();
      
      String reply = "Button " + String(buttonNum) + " set to " + phoneNumber;
      sendSMS(sender, reply);
      
      Serial.println("*** " + reply + " ***");
      printSpeedDialNumbers();
    } else {
      sendSMS(sender, "Invalid button number. Use 1-8");
    }
  } else if (upperCommand.startsWith("SMS")) {
    // Format: SMS "Hello there!" +37255639121
    Serial.println("Processing SMS relay command...");
    
    // Find the quoted message
    int firstQuote = command.indexOf('"');
    int lastQuote = command.lastIndexOf('"');
    
    if (firstQuote == -1 || lastQuote == -1 || firstQuote == lastQuote) {
      sendSMS(sender, "Invalid SMS format. Use: SMS \"message text\" +37xxxxxxx");
      return;
    }
    
    String message = command.substring(firstQuote + 1, lastQuote);
    String remainder = command.substring(lastQuote + 1);
    remainder.trim();
    
    // Extract phone number (should start with +)
    int plusPos = remainder.indexOf('+');
    if (plusPos == -1) {
      sendSMS(sender, "Invalid phone number. Must start with +");
      return;
    }
    
    String targetNumber = remainder.substring(plusPos);
    targetNumber.trim();
    
    // Validate phone number format (basic check)
    if (targetNumber.length() < 8) {
      sendSMS(sender, "Phone number too short");
      return;
    }
    
    // Send the SMS
    Serial.print("Relaying SMS to ");
    Serial.print(targetNumber);
    Serial.print(": ");
    Serial.println(message);
    
    sendSMS(targetNumber, message);
    
    // Confirm to sender
    String confirmation = "SMS sent to " + targetNumber + ": \"" + message + "\"";
    sendSMS(sender, confirmation);
    
  } else if (upperCommand == "LIST") {
    String reply = "Speed dial numbers:\n";
    for (int i = 0; i < 8; i++) {
      reply += String(i + 1) + ": " + speedDialNumbers[i] + "\n";
    }
    sendSMS(sender, reply);
    Serial.println("Sent speed dial list to " + sender);
  } else if (upperCommand == "STATUS") {
    sendSMS(sender, "Phone Zero ESP32 online. 8 buttons ready.");
    Serial.println("Sent status to " + sender);
  } else {
    // Only send help for unrecognized commands
    Serial.println("Unrecognized command: " + command);
    sendSMS(sender, "Commands: SET 1 +number, LIST, STATUS, SMS \"text\" +number");
  }
}

void sendSMS(String number, String message) {
  Serial.print("Sending SMS to ");
  Serial.print(number);
  Serial.print(": ");
  Serial.println(message);
  
  sim900.println("AT+CMGF=1"); // Text mode
  delay(1000);
  
  sim900.print("AT+CMGS=\"");
  sim900.print(number);
  sim900.println("\"");
  delay(1000);
  
  sim900.print(message);
  delay(1000);
  
  sim900.write(26); // Ctrl+Z to send
  delay(3000); // Wait for SMS to send
  
  // Check for delivery confirmation
  if (sim900.available()) {
    String response = sim900.readString();
    Serial.print("SMS Response: ");
    Serial.println(response);
    if (response.indexOf("OK") >= 0) {
      Serial.println("SMS sent successfully");
    } else {
      Serial.println("SMS send failed");
    }
  }
  
  // Clean up any remaining SMS in memory
  delay(1000);
  deleteAllSMS();
}

void deleteAllSMS() {
  Serial.println("Clearing SMS memory...");
  sim900.println("AT+CMGDA=\"DEL ALL\"");
  delay(2000);
  
  if (sim900.available()) {
    String response = sim900.readString();
    Serial.print("Delete response: ");
    Serial.println(response);
  }
}

void checkSMSMemory() {
  Serial.println("Checking SMS memory status...");
  sim900.println("AT+CPMS?");
  delay(1000);
  
  if (sim900.available()) {
    String response = sim900.readString();
    Serial.print("SMS Memory: ");
    Serial.println(response);
  }
}

void initSIM900() {
  Serial.println("Initializing SIM900A via Hardware UART2...");
  Serial.println("Using GPIO16/17 (D16/D17) for communication");
  
  // Wait for SIM900A to be fully ready
  delay(2000);
  
  // Clear any existing data
  while (sim900.available()) {
    sim900.read();
  }
  
  // Test AT command communication
  Serial.println("Testing AT commands...");
  
  for (int attempt = 1; attempt <= 3; attempt++) {
    Serial.print("AT Command attempt ");
    Serial.print(attempt);
    Serial.println("...");
    
    sim900.println("AT");
    delay(1000);
    
    String response = "";
    unsigned long startTime = millis();
    
    // Read response for 2 seconds
    while (millis() - startTime < 2000) {
      if (sim900.available()) {
        response += sim900.readString();
        break;
      }
    }
    
    Serial.print("Response: ");
    Serial.println(response);
    
    if (response.indexOf("OK") >= 0) {
      Serial.println("*** SIM900A COMMUNICATION SUCCESS ***");
      
      // Set SMS text mode
      Serial.println("Setting SMS text mode...");
      sim900.println("AT+CMGF=1");
      delay(1000);
      
      // Delete all SMS messages to start clean
      Serial.println("Clearing SMS memory...");
      sim900.println("AT+CMGDA=\"DEL ALL\"");
      delay(2000);
      
      // Enable SMS notifications (forward directly, don't store)
      Serial.println("Enabling SMS notifications...");
      sim900.println("AT+CNMI=2,2,0,0,0");
      delay(1000);
      
      // Check signal quality
      Serial.println("Checking signal quality...");
      sim900.println("AT+CSQ");
      delay(2000);
      if (sim900.available()) {
        Serial.print("Signal: ");
        Serial.println(sim900.readString());
      }
      
      // Check network registration
      Serial.println("Checking network registration...");
      sim900.println("AT+CREG?");
      delay(2000);
      if (sim900.available()) {
        Serial.print("Network: ");
        Serial.println(sim900.readString());
      }
      
      Serial.println("*** SIM900A INITIALIZATION COMPLETE ***");
      return; // Success!
    }
    
    if (attempt < 3) {
      Serial.println("No response, retrying...");
      delay(2000);
    }
  }
  
  Serial.println("*** SIM900A INITIALIZATION FAILED ***");
  Serial.println("Check connections and power supply");
}

void loadSpeedDialNumbers() {
  Serial.println("Loading speed dial numbers from flash...");
  
  for (int i = 0; i < 8; i++) {
    String key = "speed" + String(i);
    String number = preferences.getString(key.c_str(), "");
    
    if (number.length() > 0) {
      speedDialNumbers[i] = number;
      Serial.print("Loaded button ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(number);
    }
  }
}

void saveSpeedDialNumbers() {
  Serial.println("Saving speed dial numbers to flash...");
  
  for (int i = 0; i < 8; i++) {
    String key = "speed" + String(i);
    preferences.putString(key.c_str(), speedDialNumbers[i]);
  }
  
  Serial.println("Speed dial numbers saved");
}

void printSpeedDialNumbers() {
  Serial.println("\n=== CURRENT SPEED DIAL NUMBERS ===");
  for (int i = 0; i < 8; i++) {
    Serial.print("Button ");
    Serial.print(i + 1);
    Serial.print(" (GPIO");
    Serial.print(buttonPins[i]);
    Serial.print("): ");
    if (speedDialNumbers[i].length() > 0) {
      Serial.println(speedDialNumbers[i]);
    } else {
      Serial.println("(not programmed)");
    }
  }
  Serial.println("=====================================\n");
}
