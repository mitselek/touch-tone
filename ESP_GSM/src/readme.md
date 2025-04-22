# ESP_GSM Project Readme

This document provides basic AT command examples for interacting with the SIM800L module for SMS operations.

## Reading SMS Messages

* **Read a specific message:** Use the `AT+CMGR=<index>` command, where `<index>` is the numerical index of the message in storage.
  * Example: `AT+CMGR=1` reads the message at index 1.

## Listing SMS Messages

* **List messages by status:** Use the `AT+CMGL="<stat>"` command. Replace `<stat>` with one of the following:
  * `"REC UNREAD"`: Lists all received unread messages.
  * `"REC READ"`: Lists all received read messages.
  * `"ALL"`: Lists all messages in the preferred storage.
  * Example: `AT+CMGL="REC READ"`

## Deleting SMS Messages

* **Delete a specific message:** Use the `AT+CMGD=<index>` command, where `<index>` is the numerical index of the message you want to delete.
  * Example: `AT+CMGD=1` deletes the message at index 1.
* **Delete multiple messages (Optional flags - check module documentation):** Some modules support flags with `AT+CMGD` to delete multiple messages at once (e.g., delete all read messages). Refer to your specific SIM800L module's AT command manual for details.
  * Example (Common but verify): `AT+CMGD=1,1` deletes all read messages.
* **Delete ALL messages:** Use the `AT+CMGD=1,4` command. The flag `4` instructs the module to delete all messages regardless of their status (read, unread, sent, unsent).
  * Example: `AT+CMGD=1,4` deletes all messages from the preferred storage.
