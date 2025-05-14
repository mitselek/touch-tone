# Hardware Setup

This project uses an ESP32 microcontroller connected to a SIM800L GSM modem.

## Pin Layout

- **SIM800L Modem (UART2):**
  - ESP32 **GPIO 17 (TX)** <-> SIM800L **RX**
  - ESP32 **GPIO 16 (RX)** <-> SIM800L **TX**
  - Ensure common **GND** connection between ESP32 and SIM800L.
  - Provide adequate power supply to the SIM800L (typically requires 2A peak current).
- **Optional:**  
    Add a capacitor (e.g., 1000 µF or at least 100 µF) across the **SIM800L's power supply lines** to handle voltage drops during peak current usage.  
    If you have multiple capacitors (e.g., two 100 µF capacitors), you can connect them in parallel to increase the total capacitance and improve stability.  
    **Note:** If using polarized capacitors, connect the positive terminal (`+`) to **VCC** and the negative terminal (`-`) to **GND** of the **SIM800L**.

## Connection Diagram

```ascii
+------+     +-------+
|ESP32 |     |SIM800L|
|GPIO17|---->|RX     |
|GPIO16|<----|TX     |
|GND   |-----|GND    |
|      |     +-------+
+------+
   |
   +--[100 µF]--+
   |            |
   +--[100 µF]--+
   |            |
  GND          VCC (SIM800L)
```

---

## Basic AT Commands

Here is a list of commonly used AT commands for the SIM800L module:

| Command               | Description                                                                 |
|-----------------------|-----------------------------------------------------------------------------|
| `AT`                  | Test the communication with the module.                                     |
| `ATE0`                | Disable command echo.                                                       |
| `ATE1`                | Enable command echo.                                                        |
| `AT+CMGF=1`           | Set SMS mode to Text Mode.                                                  |
| `AT+CMGR=<index>`     | Read an SMS at the specified index.                                         |
| `AT+CMGD=<index>`     | Delete an SMS at the specified index.                                       |
| `AT+CMGL="<stat>"`    | List SMS messages by status (`"ALL"`, `"REC UNREAD"`, `"REC READ"`, etc.).  |
| `AT+CMGD=1,4`         | Delete all SMS messages from the preferred storage.                         |
| `AT+CNMI=2,1,0,0,0`   | Configure SMS notifications to UART.                                        |
| `AT+CHFA=1`           | Set audio output to the call channel.                                       |
| `AT+CREC=4,"<file>",0`| Play an audio file during a call.                                           |
| `AT+FSWRITE="<path>",0,<size>,<timeout>` | Write a file to the module's filesystem.                 |
| `AT+FSLS="<path>"`    | List files in the specified directory.                                      |
| `AT+DTMF=<digit>`     | Send a DTMF tone during a call.                                             |
| `AT+CFUN=1,1`         | Restart the module.                                                         |
| `ATD<phone_number>;`  | Place a voice call to the specified phone number.                           |
| `ATH`                 | Hang up (terminate) the current call.                                       |
| `ATA`                 | Answer an incoming call.                                                    |

When a call is incoming, the module will output `RING` and the caller's number (if available). Use `ATA` to answer, or `ATH` to reject/hang up.

## Network AT Commands

Here is a list of AT commands for managing and querying network-related information:

| Command               | Description                                                                 |
|-----------------------|-----------------------------------------------------------------------------|
| `AT+COPS?`           | Query the current network operator.                                         |
| `AT+COPS=?`          | List available network operators.                                           |
| `AT+CREG?`           | Check the network registration status.                                      |
| `AT+CSQ`             | Get the signal quality (RSSI and BER).                                      |
| `AT+CGATT?`          | Check if the module is attached to the GPRS network.                        |
| `AT+SAPBR=2,1`       | Query the bearer profile status (e.g., for GPRS).                           |

### Examples

1. **Query Current Network Operator**: `AT+COPS?`  
   Response example: `+COPS: 0,0,"YourNetworkName",2`

2. **List Available Network Operators**: `AT+COPS=?`  
   Response example: `+COPS: (2,"Network1","N1","00101",2),(1,"Network2","N2","00102",0)`

3. **Check Network Registration Status**: `AT+CREG?`  
   Response example: `+CREG: 0,1`
   - `0,1`: Registered to home network.
   - `0,5`: Registered to roaming network.

4. **Get Signal Quality**: `AT+CSQ`  
   Response example:`+CSQ: 15,0`
   - `15`: RSSI (Received Signal Strength Indicator).
   - `0`: BER (Bit Error Rate).

5. **Check GPRS Attachment**: `AT+CGATT?`  
   Response example: `+CGATT: 1`  
   - `1`: Attached to GPRS.
   - `0`: Not attached to GPRS.

6. **Query Bearer Profile Status**: `AT+SAPBR=2,1`  
   Response example: `+SAPBR: 1,1,"10.0.0.1"`  
   - `1,1`: Bearer is active.
   - `"10.0.0.1"`: IP address assigned to the module.

## Reading SMS Messages

- **Read a specific message:** Use the `AT+CMGR=<index>` command, where `<index>` is the numerical index of the message in storage.
  - Example: `AT+CMGR=1` reads the message at index 1.

## Listing SMS Messages

- **List messages by status:** Use the `AT+CMGL="<stat>"` command. Replace `<stat>` with one of the following:
  - `"REC UNREAD"`: Lists all received unread messages.
  - `"REC READ"`: Lists all received read messages.
  - `"ALL"`: Lists all messages in the preferred storage.
  - Example: `AT+CMGL="REC READ"`

## Deleting SMS Messages

- **Delete a specific message:** Use the `AT+CMGD=<index>` command, where `<index>` is the numerical index of the message you want to delete.
  - Example: `AT+CMGD=1` deletes the message at index 1.
- **Delete multiple messages (Optional flags - check module documentation):** Some modules support flags with `AT+CMGD` to delete multiple messages at once (e.g., delete all read messages). Refer to your specific SIM800L module's AT command manual for details.
  - Example (Common but verify): `AT+CMGD=1,1` deletes all read messages.
- **Delete ALL messages:** Use the `AT+CMGD=1,4` command. The flag `4` instructs the module to delete all messages regardless of their status (read, unread, sent, unsent).
  - Example: `AT+CMGD=1,4` deletes all messages from the preferred storage.
