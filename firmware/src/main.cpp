#include <Arduino.h>
#include <Preferences.h>

// Preferences object for persistent storage
Preferences preferences;

// Define the Serial port pins for SIM800L
#define RXD2 19
#define TXD2 18

// Define button pins (assuming pins 2-9 for 8 buttons)
#define BUTTON_1 32
#define BUTTON_2 33
#define BUTTON_3 25
#define BUTTON_4 26
#define BUTTON_5 27
#define BUTTON_6 14
#define BUTTON_7 12
#define BUTTON_8 13

// Define the Serial port instance for SIM800L
#define SIM800_SERIAL Serial2

// Speed dial numbers array (8 slots)
String speedDialNumbers[8] = {
  "+37256560978",  // Button 1
  "+",  // Button 2  
  "+",  // Button 3
  "+",  // Button 4
  "+",  // Button 5
  "+",  // Button 6
  "+",  // Button 7
  "+"   // Button 8
};

// Button pins array for easy iteration
int buttonPins[8] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8};
bool lastButtonState[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}; // Assuming pull-up
unsigned long lastDebounceTime[8] = {0, 0, 0, 0, 0, 0, 0, 0};
const unsigned long debounceDelay = 50;

// Call state
bool incomingCall = false;
bool inCall = false;

// SMS state machine
enum SmsState {
  SMS_IDLE,
  SMS_WAIT_HEADER,
  SMS_WAIT_CONTENT
};
SmsState smsState = SMS_IDLE;
String smsPhoneNumber = "";
String smsMessageContent = "";
int smsIndexToDelete = -1;

// Function declarations
void printWithTime(const String &message);
bool sendCommandAndWait(const String &command, const String &expectedResponse, unsigned long timeout = 5000);
void sendSms(const String &phoneNumber, const String &message);
void makeCall(const String &phoneNumber);
void answerCall();
void hangupCall();
void checkButtons();
void processSmsNotification(String notificationLine);
void handleSmsLine(const String &line);
void processIncomingCall();
void updateSpeedDialNumber(int index, const String &newNumber);
void processSmsCommand(const String &command);
void saveSpeedDialNumbers();
void loadSpeedDialNumbers();

// Helper function to prepend time to Serial output
void printWithTime(const String &message)
{
  Serial.print("[");
  Serial.print(millis());
  Serial.print(" ms] ");
  Serial.println(message);
}

// Helper function to send a command and wait for a response
bool sendCommandAndWait(const String &command, const String &expectedResponse, unsigned long timeout)
{
  SIM800_SERIAL.println(command);
  printWithTime("Command to SIM800L: " + command);
  
  unsigned long startTime = millis();
  String currentLine = "";
  char receivedChar;

  while (millis() - startTime < timeout)
  {
    while (SIM800_SERIAL.available())
    {
      receivedChar = SIM800_SERIAL.read();

      if (receivedChar == '\n')
      {
        currentLine.trim();
        if (currentLine.length() > 0)
        {
          printWithTime("SIM800L Raw Line: " + currentLine);
          if (currentLine.indexOf(expectedResponse) != -1)
          {
            printWithTime("SIM800L: + Expected response found in: " + currentLine);
            return true;
          }
          else
          {
            // Only print if it's not an empty line or just the command echoed back
            if (!currentLine.equalsIgnoreCase(command) && !currentLine.isEmpty())
            {
              printWithTime("SIM800L: - Unexpected line: " + currentLine);
            }
          }
        }
        currentLine = "";
      }
      else if (receivedChar != '\r')
      {
        currentLine += receivedChar;
      }
    }
    yield();
  }

  // Check if there's any partial line data at timeout
  currentLine.trim();
  if (currentLine.length() > 0)
  {
    printWithTime("SIM800L Partial Line at Timeout: " + currentLine);
    if (currentLine.indexOf(expectedResponse) != -1)
    {
      printWithTime("SIM800L: + Expected response found in partial line: " + currentLine);
      return true;
    }
    else
    {
      if (!currentLine.equalsIgnoreCase(command) && !currentLine.isEmpty())
      {
        printWithTime("SIM800L: - Unexpected partial line at timeout: " + currentLine);
      }
    }
  }

  printWithTime("Timeout waiting for '" + expectedResponse + "' in response to: " + command);
  return false;
}

// Function to send SMS
void sendSms(const String &phoneNumber, const String &message)
{
  printWithTime("Sending SMS to: " + phoneNumber);
  printWithTime("Message: " + message);

  // Set SMS to text mode
  if (!sendCommandAndWait("AT+CMGF=1", "OK", 3000))
  {
    printWithTime("Failed to set SMS text mode");
    return;
  }

  // Send SMS command and wait for '>' prompt
  if (!sendCommandAndWait("AT+CMGS=\"" + phoneNumber + "\"", ">", 5000))
  {
    printWithTime("Failed to get SMS prompt '>'");
    return;
  }
  
  printWithTime("Got SMS prompt, sending message content...");
  
  // Send message content followed by Ctrl+Z (ASCII 26)
  SIM800_SERIAL.print(message);
  printWithTime("Message content '" + message + "' sent, now sending Ctrl+Z...");
  delay(100); // Small delay before Ctrl+Z
  SIM800_SERIAL.write(26);
  printWithTime("Ctrl+Z sent (ASCII 26). Waiting for confirmation...");
  
  // Wait for response
  unsigned long startTime = millis();
  String currentLine = "";
  char receivedChar;
  
  while (millis() - startTime < 15000) // 15 second timeout for SMS sending
  {
    while (SIM800_SERIAL.available())
    {
      receivedChar = SIM800_SERIAL.read();

      if (receivedChar == '\n')
      {
        currentLine.trim();
        if (currentLine.length() > 0)
        {
          printWithTime("SIM800L: " + currentLine);
          if (currentLine.indexOf("+CMGS:") != -1)
          {
            printWithTime("SMS sent successfully!");
            return;
          }
          if (currentLine.indexOf("ERROR") != -1)
          {
            printWithTime("SMS sending failed!");
            return;
          }
        }
        currentLine = "";
      }
      else if (receivedChar != '\r')
      {
        currentLine += receivedChar;
      }
    }
    yield();
  }
  
  printWithTime("SMS sending timeout");
}

// Function to make a call
void makeCall(const String &phoneNumber)
{
  if (phoneNumber.length() == 0 || phoneNumber == "+1234567890") {
    printWithTime("Empty or default number - call cancelled");
    return;
  }
  
  printWithTime("Making call to: " + phoneNumber);
  
  if (sendCommandAndWait("ATD" + phoneNumber + ";", "OK", 5000)) {
    printWithTime("Call initiated successfully");
    inCall = true;
  } else {
    printWithTime("Failed to initiate call");
  }
}

// Function to answer incoming call
void answerCall()
{
  printWithTime("Answering incoming call...");
  if (sendCommandAndWait("ATA", "OK", 3000)) {
    printWithTime("Call answered successfully");
    incomingCall = false;
    inCall = true;
  } else {
    printWithTime("Failed to answer call");
  }
}

// Function to hang up call
void hangupCall()
{
  printWithTime("Hanging up call...");
  if (sendCommandAndWait("ATH", "OK", 3000)) {
    printWithTime("Call ended successfully");
    inCall = false;
    incomingCall = false;
  } else {
    printWithTime("Failed to hang up call");
  }
}

// Function to update speed dial number
void updateSpeedDialNumber(int index, const String &newNumber)
{
  if (index >= 1 && index <= 8) {
    String oldNumber = speedDialNumbers[index - 1];
    speedDialNumbers[index - 1] = newNumber;
    printWithTime("Speed dial " + String(index) + " updated: " + oldNumber + " → " + newNumber);
    
    // Save to flash memory immediately
    saveSpeedDialNumbers();
  } else {
    printWithTime("Invalid speed dial index: " + String(index));
  }
}

// Function to check button presses
void checkButtons()
{
  static bool buttonPressed[8] = {false}; // Track if button press has been processed
  
  for (int i = 0; i < 8; i++) {
    bool buttonState = digitalRead(buttonPins[i]);
    
    // Check if button state changed
    if (buttonState != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
      buttonPressed[i] = false; // Reset the pressed flag when state changes
    }
    
    // If button is LOW (pressed) and has been stable for debounce period
    unsigned long timeSinceChange = millis() - lastDebounceTime[i];
    if (buttonState == LOW && timeSinceChange > debounceDelay && !buttonPressed[i]) {
      buttonPressed[i] = true; // Mark as processed to avoid repeat triggers
      printWithTime("Button " + String(i + 1) + " pressed");
      
      if (incomingCall) {
        // Any button answers incoming call
        answerCall();
      } else if (inCall) {
        // Any button hangs up during call
        hangupCall();
      } else {
        // Make call to speed dial number
        printWithTime("Calling speed dial " + String(i + 1) + ": " + speedDialNumbers[i]);
        makeCall(speedDialNumbers[i]);
      }
    }
    
    lastButtonState[i] = buttonState;
  }
}

// Function to process SMS notifications
void processSmsNotification(String notificationLine)
{
  printWithTime("*** New SMS Notification Received! ***");
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
      
      // Set SMS to text mode first
      if (sendCommandAndWait("AT+CMGF=1", "OK", 3000)) {
        printWithTime("SMS text mode set successfully");
        // Send read command but DON'T wait for response - let main loop handle it
        SIM800_SERIAL.println("AT+CMGR=" + String(idx));
        printWithTime("Command to SIM800L: AT+CMGR=" + String(idx));
        printWithTime("SMS state set to WAIT_HEADER - main loop will process response");
      } else {
        printWithTime("Failed to set SMS text mode");
      }
    }
  }
}

// Function to handle SMS content processing
void handleSmsLine(const String &line)
{
  switch (smsState) {
    case SMS_WAIT_HEADER:
      if (line.startsWith("+CMGR:")) {
        // Format: +CMGR: "REC UNREAD","+37256560978","","25/04/23,02:00:30+12"
        int firstQuote = line.indexOf("\",\"") + 3;
        int secondQuote = line.indexOf("\"", firstQuote);
        if (firstQuote > 1 && secondQuote > firstQuote) {
          smsPhoneNumber = line.substring(firstQuote, secondQuote);
          printWithTime("SMS from: " + smsPhoneNumber);
          smsState = SMS_WAIT_CONTENT;
        }
      }
      break;
      
    case SMS_WAIT_CONTENT:
      printWithTime("SMS_WAIT_CONTENT: Processing line '" + line + "'");
      if (line.equals("OK")) {
        printWithTime("Got OK - finishing SMS processing");
        // Process the SMS if we have content
        if (smsPhoneNumber.length() > 0 && smsMessageContent.length() > 0) {
          printWithTime("Processing SMS from " + smsPhoneNumber + ": '" + smsMessageContent + "'");
          processSmsCommand(smsMessageContent);
        } else {
          printWithTime("ERROR: Missing data - Phone: '" + smsPhoneNumber + "', Content: '" + smsMessageContent + "'");
        }
        
        // IMMEDIATELY delete the SMS to prevent buffer overflow
        if (smsIndexToDelete > 0) {
          printWithTime("Deleting SMS at index: " + String(smsIndexToDelete));
          if (sendCommandAndWait("AT+CMGD=" + String(smsIndexToDelete), "OK", 3000)) {
            printWithTime("SMS deleted successfully");
          } else {
            printWithTime("Failed to delete SMS");
          }
        }
        
        // Reset state
        printWithTime("Resetting SMS state machine");
        smsState = SMS_IDLE;
        smsPhoneNumber = "";
        smsMessageContent = "";
        smsIndexToDelete = -1;
      } else if (!line.isEmpty() && smsMessageContent.isEmpty()) {
        // This should be the SMS content line
        smsMessageContent = line;
        smsMessageContent.trim(); // Remove any whitespace
        printWithTime("SMS content captured: '" + smsMessageContent + "'");
      } else {
        printWithTime("SMS_WAIT_CONTENT: Ignoring line '" + line + "'");
      }
      break;
      
    default:
      break;
  }
}

// Function to process SMS commands for speed dial configuration
void processSmsCommand(const String &command)
{
  String cmd = command;
  cmd.trim();
  cmd.toUpperCase(); // Make it case-insensitive
  
  printWithTime("Processing SMS command: '" + cmd + "'");
  
  // Expected format: "SET 1 +1234567890" (set speed dial 1 to +1234567890)
  if (cmd.startsWith("SET ")) {
    // Find spaces to parse: "SET 3 +37255639121"
    //                            ^   ^
    //                            |   second space
    //                            first space
    int firstSpace = 3; // Space after "SET"
    int secondSpace = cmd.indexOf(' ', firstSpace + 1);
    
    printWithTime("Parsing: firstSpace=" + String(firstSpace) + ", secondSpace=" + String(secondSpace));
    
    if (secondSpace != -1) {
      String indexStr = cmd.substring(firstSpace + 1, secondSpace); // Between first and second space
      String newNumber = command.substring(secondSpace + 1); // After second space (use original case)
      newNumber.trim();
      
      int index = indexStr.toInt();
      printWithTime("Parsed: indexStr='" + indexStr + "' (" + String(index) + "), number='" + newNumber + "'");
      
      if (index >= 1 && index <= 8 && newNumber.length() > 0) {
        updateSpeedDialNumber(index, newNumber);
        
        // Send confirmation SMS (when you have credit)
        String confirmMsg = "Speed dial " + String(index) + " set to " + newNumber;
        printWithTime("SUCCESS: " + confirmMsg);
        // sendSms(smsPhoneNumber, confirmMsg); // Uncomment when you have SMS credit
      } else {
        printWithTime("ERROR: Invalid index (" + String(index) + ") or empty number");
      }
    } else {
      printWithTime("ERROR: Could not find second space in SET command");
    }
  } else if (cmd.equals("LIST")) {
    // List all speed dial numbers
    printWithTime("=== CURRENT SPEED DIAL NUMBERS ===");
    for (int i = 0; i < 8; i++) {
      printWithTime(String(i + 1) + ": " + speedDialNumbers[i]);
    }
    
    // Send list via SMS (when you have credit)
    String listMsg = "Speed dials:\n";
    for (int i = 0; i < 8; i++) {
      listMsg += String(i + 1) + ": " + speedDialNumbers[i] + "\n";
    }
    printWithTime("Would send list via SMS (no credit)");
    // sendSms(smsPhoneNumber, listMsg); // Uncomment when you have SMS credit
  } else if (cmd.equals("SAVE")) {
    // Manually save speed dial numbers
    saveSpeedDialNumbers();
    printWithTime("Manual save completed");
  } else {
    printWithTime("ERROR: Unknown command '" + cmd + "'. Use 'SET n +number', 'LIST', or 'SAVE'");
  }
}

// Function to handle incoming call notifications
void processIncomingCall()
{
  printWithTime("*** INCOMING CALL! Press any button to answer ***");
  incomingCall = true;
}

// Function to save speed dial numbers to flash memory
void saveSpeedDialNumbers()
{
  printWithTime("Saving speed dial numbers to flash memory...");
  
  // Open preferences with our namespace
  if (preferences.begin("speeddial", false)) { // false = read-write mode
    bool success = true;
    
    // Save each speed dial number with a unique key
    for (int i = 0; i < 8; i++) {
      String key = "dial" + String(i + 1); // dial1, dial2, etc.
      if (preferences.putString(key.c_str(), speedDialNumbers[i]) == 0) {
        printWithTime("Failed to save speed dial " + String(i + 1));
        success = false;
      }
    }
    
    preferences.end(); // Always close preferences
    
    if (success) {
      printWithTime("All speed dial numbers saved successfully");
    } else {
      printWithTime("Some speed dial numbers failed to save");
    }
  } else {
    printWithTime("ERROR: Failed to open preferences for saving");
  }
}

// Function to load speed dial numbers from flash memory
void loadSpeedDialNumbers()
{
  printWithTime("Loading speed dial numbers from flash memory...");
  
  // Open preferences with our namespace
  if (preferences.begin("speeddial", true)) { // true = read-only mode
    bool foundSaved = false;
    
    // Load each speed dial number
    for (int i = 0; i < 8; i++) {
      String key = "dial" + String(i + 1); // dial1, dial2, etc.
      String savedNumber = preferences.getString(key.c_str(), ""); // Empty string as default
      
      if (savedNumber.length() > 0) {
        speedDialNumbers[i] = savedNumber;
        printWithTime("Loaded speed dial " + String(i + 1) + ": " + savedNumber);
        foundSaved = true;
      } else {
        // Keep default values for empty slots
        printWithTime("Speed dial " + String(i + 1) + ": using default " + speedDialNumbers[i]);
      }
    }
    
    preferences.end(); // Always close preferences
    
    if (foundSaved) {
      printWithTime("Speed dial numbers loaded from flash memory");
    } else {
      printWithTime("No saved speed dial numbers found, using defaults");
    }
  } else {
    printWithTime("WARNING: Failed to open preferences for loading, using defaults");
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }
  printWithTime("Serial Monitor Initialized");

  // Initialize button pins as inputs with pull-up resistors
  for (int i = 0; i < 8; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  printWithTime("8 buttons initialized on pins 34,35,32,33,25,26,27,14");

  // Load saved speed dial numbers from flash memory
  loadSpeedDialNumbers();

  // Initialize SIM800L Serial
  SIM800_SERIAL.begin(57600, SERIAL_8N1, RXD2, TXD2);
  printWithTime("SIM800L Serial Initialized on pins RX: " + String(RXD2) + ", TX: " + String(TXD2));
  
  delay(500); // Short initial delay

  // Try to communicate with SIM800L as soon as possible
  printWithTime("Checking if SIM800L is already responsive...");
  if (sendCommandAndWait("AT", "OK", 2000)) {
    printWithTime("SIM800L is responsive to AT command (no long wait needed).");
  } else {
    printWithTime("SIM800L not responsive yet, waiting 3 seconds and retrying...");
    delay(3000);
    if (sendCommandAndWait("AT", "OK", 3000)) {
      printWithTime("SIM800L is now responsive after short wait.");
    } else {
      printWithTime("SIM800L did not respond as expected. Trying different baud rates...");
      
      // Try common baud rates
      int baudRates[] = {9600, 19200, 38400, 57600, 115200};
      bool foundBaud = false;
      
      for (int i = 0; i < 5; i++) {
        int baud = baudRates[i];
        SIM800_SERIAL.end();
        delay(100);
        SIM800_SERIAL.begin(baud, SERIAL_8N1, RXD2, TXD2);
        printWithTime("Testing SIM800L at baud: " + String(baud));
        delay(3000); // Give SIM800L more time to adjust - matching inspiration.cpp
        
        if (sendCommandAndWait("AT", "OK", 2000)) {
          printWithTime("SUCCESS: SIM800L responded at baud " + String(baud));
          foundBaud = true;
          break;
        } else {
          printWithTime("No response at baud " + String(baud));
        }
      }
      
      if (!foundBaud) {
        printWithTime("Failed to find working baud rate. Check connections and power.");
        printWithTime("Dumping raw serial from SIM800L for 5 seconds...");
        unsigned long debugStartTime = millis();
        bool receivedData = false;
        while (millis() - debugStartTime < 5000) {
          if (SIM800_SERIAL.available()) {
            Serial.write(SIM800_SERIAL.read());
            receivedData = true;
          }
          yield();
        }
        if (receivedData) {
          Serial.println("\nRaw data dump complete.");
        } else {
          Serial.println("\nNo raw data received from SIM800L during dump.");
        }
        printWithTime("Halting due to SIM800L communication failure.");
        while (1) { yield(); }
      }
    }
  }

  // Disable command echo
  if (sendCommandAndWait("ATE0", "OK", 2000)) {
    printWithTime("Command echo disabled (ATE0).");
  } else {
    printWithTime("Failed to disable command echo. Continuing anyway...");
  }

  // Check SIM card
  if (!sendCommandAndWait("AT+CPIN?", "+CPIN: READY", 5000))
  {
    printWithTime("SIM card not ready - check if PIN is required");
  }

  // Check network registration
  sendCommandAndWait("AT+CREG?", "+CREG:", 5000);
  
  // Check signal quality
  sendCommandAndWait("AT+CSQ", "+CSQ:", 3000);

  // Set SMS to text mode for receiving
  if (sendCommandAndWait("AT+CMGF=1", "OK", 3000)) {
    printWithTime("SMS text mode enabled");
  } else {
    printWithTime("Failed to enable SMS text mode");
  }

  // Enable SMS notifications
  if (sendCommandAndWait("AT+CNMI=2,1,0,0,0", "OK", 3000)) {
    printWithTime("SMS notifications enabled");
  } else {
    printWithTime("Failed to enable SMS notifications");
  }

  printWithTime("SIM800L initialized successfully!");
  printWithTime("=== SPEED DIAL PHONE READY ===");
  printWithTime("- Press buttons 1-8 to call speed dial numbers");
  printWithTime("- Press any button to answer incoming calls");
  printWithTime("- Send SMS 'SET n +number' to change speed dial n");
  printWithTime("- Send SMS 'LIST' to see all speed dial numbers");
  printWithTime("- Send SMS 'SAVE' to manually save numbers to flash");
  printWithTime("Current speed dial numbers:");
  for (int i = 0; i < 8; i++) {
    printWithTime("  " + String(i + 1) + ": " + speedDialNumbers[i]);
  }
  printWithTime("--------------------------------------------------");
}

void loop()
{
  // Check button presses
  checkButtons();
  
  // Handle SIM800L responses
  if (SIM800_SERIAL.available())
  {
    String line = SIM800_SERIAL.readStringUntil('\n');
    line.trim();
    if (line.length() > 0)
    {
      printWithTime("SIM800L: " + line);
      
      // Handle SMS state machine
      if (smsState == SMS_WAIT_HEADER || smsState == SMS_WAIT_CONTENT) {
        handleSmsLine(line);
      }
      
      // Check for new SMS notification
      if (line.startsWith("+CMTI:")) {
        processSmsNotification(line);
      }
      
      // Check for incoming call
      if (line.startsWith("RING")) {
        processIncomingCall();
      }
      
      // Check for call ended
      if (line.equalsIgnoreCase("NO CARRIER")) {
        printWithTime("Call disconnected");
        inCall = false;
        incomingCall = false;
      }
    }
  }

  // Handle Serial Monitor input (for manual AT commands and SMS testing)
  if (Serial.available())
  {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.startsWith("SMS:"))
    {
      // Parse SMS command: SMS:+1234567890:Hello World
      int firstColon = input.indexOf(':', 4); // Find colon after phone number
      
      if (firstColon != -1)
      {
        String phoneNumber = input.substring(4, firstColon);
        String content = input.substring(firstColon + 1);
        
        if (phoneNumber.length() > 0 && content.length() > 0)
        {
          // sendSms(phoneNumber, content);
        }
        else
        {
          printWithTime("Invalid SMS format. Use: SMS:<phone>:<message>");
        }
      }
      else
      {
        printWithTime("Invalid SMS format. Use: SMS:<phone>:<message>");
      }
    }
    else if (input.startsWith("CALL:"))
    {
      // Manual call command: CALL:+1234567890
      String phoneNumber = input.substring(5);
      phoneNumber.trim();
      if (phoneNumber.length() > 0) {
        makeCall(phoneNumber);
      }
    }
    else if (input.equalsIgnoreCase("ANSWER"))
    {
      answerCall();
    }
    else if (input.equalsIgnoreCase("HANGUP"))
    {
      hangupCall();
    }
    else
    {
      // Forward other AT commands directly to SIM800L
      SIM800_SERIAL.println(input);
      printWithTime("Command to SIM800L: " + input);
    }
  }
}