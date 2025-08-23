#include <Arduino.h>

// Define the Serial port pins for SIM800L
#define RXD2 19
#define TXD2 18

// Define the Serial port instance for SIM800L
#define SIM800_SERIAL Serial2

// Function declarations
void printWithTime(const String &message);
bool sendCommandAndWait(const String &command, const String &expectedResponse, unsigned long timeout = 5000);
void sendSms(const String &phoneNumber, const String &message);

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

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }
  printWithTime("Serial Monitor Initialized");

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

  printWithTime("SIM800L initialized successfully!");
  printWithTime("Type 'SMS:+1234567890:Hello World' to send an SMS");
  printWithTime("Format: SMS:<phone_number>:<message>");
  printWithTime("--------------------------------------------------");
}

void loop()
{
  // Handle SIM800L responses
  if (SIM800_SERIAL.available())
  {
    String line = SIM800_SERIAL.readStringUntil('\n');
    line.trim();
    if (line.length() > 0)
    {
      printWithTime("SIM800L: " + line);
    }
  }

  // Handle Serial Monitor input
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
          sendSms(phoneNumber, content);
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
    else
    {
      // Forward other AT commands directly to SIM800L
      SIM800_SERIAL.println(input);
      printWithTime("Command to SIM800L: " + input);
    }
  }
}