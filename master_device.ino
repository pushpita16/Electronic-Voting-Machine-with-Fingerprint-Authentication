/*
 * Electronic Voting Machine with Fingerprint Authentication
 * MASTER DEVICE - Arduino UNO
 *
 * Receives the selected candidate ID from the slave device over UART,
 * increments that candidate's vote count, and cycles the running totals
 * on a 2.8" ILI9341 TFT display.

 */

#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SoftwareSerial.h>

#define TFT_CS   A5
#define TFT_RST  A4
#define TFT_DC   A3
#define TFT_MOSI 11
#define TFT_CLK  12
#define TFT_MISO 13

Adafruit_ILI9341 tft = Adafruit_ILI9341(
    TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

const char* candidates[] = {
    "Alice Ahmed",
    "Shibli Noman",
    "Chayan Chowdhury",
    "Dalia Khatun"
};
int selectedCandidate = 0;

const unsigned char coffee_logo   [] PROGMEM = {...};   // 64x64 pixel values
const unsigned char shapla_logo   [] PROGMEM = {...};   // 64x64 pixel values
const unsigned char elephant_logo [] PROGMEM = {...};   // 64x64 pixel values
const unsigned char bird_logo     [] PROGMEM = {...};   // 64x64 pixel values

const int totalCandidates = sizeof(candidates) / sizeof(candidates[0]);
int votes[totalCandidates];

SoftwareSerial softSerial(4, 3);   // RX, TX


void setup() {
    Serial.begin(9600);
    softSerial.begin(9600);

    for (int i = 0; i < totalCandidates; i++) votes[i] = 0;

    Serial.println("Initializing display...");
    tft.begin();
    tft.setRotation(0);        // Landscape
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextWrap(false);
    tft.setTextSize(2);
    Serial.println("Display ready.");
    displayCandidates();
}


void loop() {

    if (softSerial.available()) {
        int r = softSerial.read();
        Serial.print("Vote for ");
        Serial.println(candidates[r - 1]);
        votes[r - 1]++;

        Serial.println("Total Votes: ");
        for (int i = 0; i < totalCandidates; i++) {
            Serial.print(candidates[i]);
            Serial.print(": ");
            Serial.print(votes[i]);
            Serial.println(" votes");
        }
        Serial.println(" ");
    }

    selectedCandidate++;
    if (selectedCandidate >= totalCandidates) selectedCandidate = 0;
    displayCandidates();
    delay(5000);
}


void displayCandidates() {
    tft.fillScreen(ILI9341_BLACK);   // Clear screen

    if      (selectedCandidate == 0) tft.drawBitmap(10, 30, coffee_logo,   64, 64, ILI9341_WHITE);
    else if (selectedCandidate == 1) tft.drawBitmap(10, 30, shapla_logo,   64, 64, ILI9341_WHITE);
    else if (selectedCandidate == 2) tft.drawBitmap(10, 30, elephant_logo, 64, 64, ILI9341_WHITE);
    else if (selectedCandidate == 3) tft.drawBitmap(10, 30, bird_logo,     64, 64, ILI9341_WHITE);

    tft.setCursor(10, 110);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.println(candidates[selectedCandidate]);

    tft.setCursor(10, 140);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.print("Total votes: ");
    tft.println(votes[selectedCandidate]);
}
