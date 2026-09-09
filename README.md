# Electronic-Voting-Machine-with-Fingerprint-Authentication
Developed an Arduino-based electronic voting system with fingerprint authentication. Integrated UART communication and real-time vote counting using a master–slave embedded architecture, where the slave device authenticates the voter and captures the candidate selection while the master maintains and displays live vote counts.

## How it works

1. The voter places a finger on the R307 sensor. If the print matches an enrolled voter who hasn't voted, the TFT shows "You can vote"; if they already have, it shows "Already Voted".
2. The voter scrolls the candidate list with a rotary encoder and confirms with its push button.
3. The candidate ID is sent to the master over UART at 9600 baud.
4. The master increments that candidate's count and updates its display in real time.


## Hardware

**Slave device**
- Arduino Mega
- R307 fingerprint sensor
- 2.8" ILI9341 TFT display
- KY-040 rotary encoder

**Master device**
- Arduino UNO
- 2.8" ILI9341 TFT display

## Software

- Arduino (C++), Arduino IDE
- Library used - `Adafruit_Fingerprint`, `Adafruit_GFX`, `Adafruit_ILI9341`

## Course

EEE 416 — Microprocessor and Embedded System Laboratory, January 2025

Department of Electrical and Electronic Engineering, BUET
