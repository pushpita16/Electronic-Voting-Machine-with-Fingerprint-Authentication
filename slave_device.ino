/*
 * Electronic Voting Machine with Fingerprint Authentication
 * SLAVE DEVICE - Arduino Mega
 *
 * Authenticates the voter with an R307 fingerprint sensor, shows the candidate
 * list on a 2.8" ILI9341 TFT, takes the selection from a rotary encoder, and
 * sends the chosen candidate ID to the master device over UART.
 */

#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial3
#endif

//SoftwareSerial softSerial(4, 3);   // RX, TX (only TX used)

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// === SPI Pin Definitions for Arduino Mega ===
#define TFT_CS   A5
#define TFT_RST  A4
#define TFT_DC   A3
#define TFT_MOSI 48
#define TFT_CLK  50
#define TFT_MISO 52

int encoderPinA = 5;   // CLK pin
int encoderPinB = 6;   // DT pin
int encoderBtn  = 7;   // SW pin
int count = 0;

const int voters_count = 9;
int encoderPinA_prev;
int encoderPinA_value;
int voters[voters_count] = {0};
int current_voter = -1;
int matched = 0;
boolean bool_CW;

// Use full 6-argument constructor for Mega
Adafruit_ILI9341 tft = Adafruit_ILI9341(
    TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// === Candidate list ===
const char* candidates[] = {
    "Alice Ahmed",
    "Shibli Noman",
    "Chayan Chowdhury",
    "Dalia Khatun"
};

const unsigned char coffee_logo   [] PROGMEM = {...};   // 64x64 pixel values
const unsigned char shapla_logo   [] PROGMEM = {...};   // 64x64 pixel values
const unsigned char elephant_logo [] PROGMEM = {...};   // 64x64 pixel values
const unsigned char bird_logo     [] PROGMEM = {...};   // 64x64 pixel values

const int totalCandidates = sizeof(candidates) / sizeof(candidates[0]);
int selectedCandidate = 0;


void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    //softSerial.begin(9600);

    pinMode(encoderPinA, INPUT);
    pinMode(encoderPinB, INPUT);
    pinMode(encoderBtn, INPUT_PULLUP);
    encoderPinA_prev = digitalRead(encoderPinA);

    Serial.println("Initializing display...");
    tft.begin();
    tft.setRotation(0);        // Landscape
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextWrap(false);
    tft.setTextSize(2);
    Serial.println("Display ready.");
    displayCandidates();

    finger.begin(57600);
    if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    } else {
        Serial.println("Did not find fingerprint sensor :(");
        while (1) { delay(1); }
    }
}


void loop() {

    int id = getFingerprintID();

    if (id != 0 && voters[id] == 0) {
        matched = 1;
        current_voter = id;
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(10, 30);
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        tft.println("You can vote");
        Serial.println("You can vote! ");
        Serial.println(" ");
        delay(4000);
        displayCandidates();
    }
    else if (voters[id] != 0) {
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(10, 30);
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        tft.println("Already Voted!!!!");
        Serial.println("Already Voted!!!!");
        Serial.println(" ");
        delay(4000);
        displayCandidates();
    }

    if (matched == 1) {
        encoderPinA_value = digitalRead(encoderPinA);

        if (encoderPinA_value != encoderPinA_prev) {   // check if knob is rotating
            // if pin A state changed before pin B, rotation is clockwise
            if (digitalRead(encoderPinB) != encoderPinA_value) {
                selectedCandidate++;
                if (selectedCandidate >= totalCandidates) selectedCandidate = 0;
            } else {
                // if pin B state changed before pin A, rotation is counter-clockwise
                selectedCandidate--;
                if (selectedCandidate < 0) selectedCandidate = totalCandidates - 1;
            }

            Serial.println(candidates[selectedCandidate]);
            displayCandidates();

            delay(1000);
            encoderPinA_value = digitalRead(encoderPinA);
        }

        if (digitalRead(encoderBtn) == LOW) {
            Serial.print(candidates[selectedCandidate]);
            Serial.println(" is selected!");
            Serial.println(" ");
            Serial1.write(selectedCandidate + 1);
            delay(5000);
            selectedCandidate = 0;
            displayCandidates();
            delay(2000);
            encoderPinA_value = digitalRead(encoderPinA);
            matched = 0;
            voters[current_voter] = 1;
        }

        encoderPinA_prev = encoderPinA_value;
    }
}


void displayCandidates() {
    tft.fillScreen(ILI9341_BLACK);   // Clear screen

    if      (selectedCandidate == 0) tft.drawBitmap(10, 30, coffee_logo,   64, 64, ILI9341_WHITE);
    else if (selectedCandidate == 1) tft.drawBitmap(10, 30, shapla_logo,   64, 64, ILI9341_WHITE);
    else if (selectedCandidate == 2) tft.drawBitmap(10, 30, elephant_logo, 64, 64, ILI9341_WHITE);
    else if (selectedCandidate == 3) tft.drawBitmap(10, 30, bird_logo,     64, 64, ILI9341_WHITE);

    tft.setCursor(10, 140);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.println(candidates[selectedCandidate]);
}


uint8_t getFingerprintID() {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK) return 0;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) return 0;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK) return 0;

    return finger.fingerID;
}
