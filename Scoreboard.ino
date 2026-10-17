/* ----------------------------------------------------------------------
https://learn.adafruit.com/adafruit-gfx-graphics-library
------------------------------------------------------------------------- */

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF

#include <Adafruit_Protomatter.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "ArduinoHttpClient.h"

struct SportEvent
{
  String eventName;
  String drivesRef;
  int week;
  JsonDocument homeTeam;
  JsonDocument awayTeam;
};

uint8_t rgbPins[] = {2,3,4,5,8,9};
uint8_t addrPins[] = {10,16,18,20,22};
uint8_t clockPin = 11;
uint8_t latchPin = 12;
uint8_t oePin = 13;

Adafruit_Protomatter matrix(
  64,          // Width of matrix (or matrix chain) in pixels
  4,           // Bit depth, 1-6
  1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
  5, addrPins, // # of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  false);      // No double-buffering here (see "doublebuffer" example)


char ssid[] = "FlameoHotman2";      //  your network SSID (name)
char pass[] = "MitchBerg";   // your network password

char apiBase[] = "sports.core.api.espn.com";
int port = 80;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, apiBase, port);
int status = WL_IDLE_STATUS;

const int DOWN_BTN_PIN = 15;
const int UP_BTN_PIN = 28;
const int ENTER_BTN_PIN = 27;

volatile bool downBtnState = LOW;
volatile bool lastDownState = LOW;
unsigned long lastDownDebounced = 0;
bool downCanUpdate = true;

volatile bool upBtnState = LOW;
volatile bool lastUpState = LOW;
unsigned long lastUpDebounced = 0;
bool upCanUpdate = true;

volatile bool enterBtnState = LOW;
volatile bool lastEnterState = LOW;
unsigned long lastEnterDebounced = 0;
bool enterCanUpdate = true;

const unsigned long debounceDelay = 500;
const unsigned long scoreUpdateDelay = 60000;

unsigned long lastScoreUpdateTime;

void (*downBtnFunc)();
void (*upBtnFunc)();
void (*enterBtnFunc)();

void (*updateScoreFunc)();

SportEvent nflEvents[20];
int currentNflEventIndex = 0;
JsonArray driveList;
JsonArray plays;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Test");
  // Initialize matrix...
  ProtomatterStatus proStatus = matrix.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)proStatus);
  if(proStatus != PROTOMATTER_OK) {
    // DO NOT CONTINUE if matrix setup encountered an error.
    for(;;);
  }

  setupUI();

  setupWifi();

  nflSetup();

  showGameOptionsScreen();
}

// LOOP - RUNS REPEATEDLY AFTER SETUP --------------------------------------

void loop(void) {
  // Debouncing buttons
  //Down
  if ((millis() - lastDownDebounced) > debounceDelay)
  {
    downCanUpdate = true;
  }

  //Up
  if ((millis() - lastUpDebounced) > debounceDelay)
  {
    upCanUpdate = true;
  }

  //Enter
  if ((millis() - lastUpDebounced) > debounceDelay)
  {
    enterCanUpdate = true;
  }

  if ((millis() - lastScoreUpdateTime) > scoreUpdateDelay)
  {
    lastScoreUpdateTime = millis();
    updateScoreFunc();
  }
}
