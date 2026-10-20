/*
Line Scan Camera Firmware
Written by HNTech Software
*/

#include <TFT_eSPI.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>

// --- SD card on its own dedicated HSPI bus (separate from TFT's VSPI) ---
#define SD_CS    27
#define SD_SCK   14
#define SD_MISO  12
#define SD_MOSI  13

SPIClass hspi(HSPI);

// ICONS FOR UI
// 16x16 Camera Icon (Simple shape)
const uint16_t camera_icon_array[] PROGMEM = {
  0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
  0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

// 16x16 "Eye" Icon for View Photos
const uint16_t view_icon_array[] PROGMEM = {
  0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
  0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
  0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
  0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF,
  0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,
  0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,
  0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,
  0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,
  0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,
  0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF,
  0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
  0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
  0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

// Initialize the library
TFT_eSPI tft = TFT_eSPI();

// --- Global Objects ---
TFT_eSprite header = TFT_eSprite(&tft);
TFT_eSprite footer = TFT_eSprite(&tft);
TFT_eSprite btnCam = TFT_eSprite(&tft);
TFT_eSprite btnView = TFT_eSprite(&tft);

// --- Test Mode ---
// Set to false once you've wired the real buttons.
#define SERIAL_TEST_MODE true

// --- Button Pins (used when SERIAL_TEST_MODE is false) ---
#define PIN_SELECT  0
#define PIN_SHUTTER 1
#define DEBOUNCE_MS 200

bool sdCardReady = false;
float sdPercentUsed = 0;
float sdUsedGB = 0;
float sdTotalGB = 0;

// Recursively sums the size of every file on the card.
uint64_t getUsedBytes(File dir) {
  uint64_t total = 0;
  File entry = dir.openNextFile();
  while (entry) {
    if (entry.isDirectory()) {
      total += getUsedBytes(entry);
    } else {
      total += entry.size();
    }
    entry.close();
    entry = dir.openNextFile();
  }
  return total;
}

// --- App State ---
enum AppState {
  STATE_MAIN_MENU,
  STATE_TAKE_PHOTO,
  STATE_VIEW_PHOTOS
};

AppState currentState = STATE_MAIN_MENU;
int menuSelection = 0; // 0 = Take Photo, 1 = View Photos

// --- Debounce tracking (physical buttons) ---
unsigned long lastSelectPress = 0;
unsigned long lastShutterPress = 0;
bool lastSelectReading = HIGH;
bool lastShutterReading = HIGH;

void setup() {
  Serial.begin(115200);
  delay(1000);
  tft.init();
  tft.setRotation(1); // Optional: Set landscape mode
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);

  #if !SERIAL_TEST_MODE
  pinMode(PIN_SELECT, INPUT_PULLUP);
  pinMode(PIN_SHUTTER, INPUT_PULLUP);
  #endif

  #if SERIAL_TEST_MODE
  Serial.println("=== SERIAL TEST MODE ===");
  Serial.println("Send 's' = SELECT button");
  Serial.println("Send 'w' or space = SHUTTER button");
  Serial.println("=========================");
  #endif
  updateSDCardStats(); // run once at boot to populate the SD stats

  drawMainMenu();
}

void loop() {
  bool selectPressed = false;
  bool shutterPressed = false;

  #if SERIAL_TEST_MODE
    checkSerialInput(selectPressed, shutterPressed);
  #else
    selectPressed = checkButtonPress(PIN_SELECT, lastSelectReading, lastSelectPress);
    shutterPressed = checkButtonPress(PIN_SHUTTER, lastShutterReading, lastShutterPress);
  #endif

  switch (currentState) {

    case STATE_MAIN_MENU:
      if (selectPressed) {
        menuSelection = 1 - menuSelection; // toggle between 0 and 1
        drawMainMenu();
        Serial.println(menuSelection == 0 ? ">> Selected: TAKE PHOTO" : ">> Selected: VIEW PHOTOS");
      }
      if (shutterPressed) {
        if (menuSelection == 0) {
          currentState = STATE_TAKE_PHOTO;
          drawTakePhotoScreen();
          Serial.println(">> Entered: TAKE PHOTO screen");
        } else {
          currentState = STATE_VIEW_PHOTOS;
          drawViewPhotosScreen();
          Serial.println(">> Entered: VIEW PHOTOS screen");
        }
      }
      break;

    case STATE_TAKE_PHOTO:
      if (shutterPressed) {
        Serial.println(">> Calling takephoto()...");
        takephoto(); // your function - runs to completion, then we return home
        Serial.println(">> takephoto() finished, returning to menu");
        currentState = STATE_MAIN_MENU;
        drawMainMenu();
      }
      break;

    case STATE_VIEW_PHOTOS:
      if (selectPressed) {
        // TODO: cycle through photos later
        Serial.println(">> (SELECT pressed - photo cycling not implemented yet)");
      }
      if (shutterPressed) {
        currentState = STATE_MAIN_MENU;
        drawMainMenu();
        Serial.println(">> Returned to main menu");
      }
      break;
  }
}

// --- Serial Input Helper (test mode) ---
void checkSerialInput(bool &selectPressed, bool &shutterPressed) {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 's' || c == 'S') {
      selectPressed = true;
    } else if (c == 'w' || c == 'W' || c == ' ') {
      shutterPressed = true;
    }
  }
}

// --- Physical Button Helper ---
// Returns true once per physical press (falling edge), debounced.
bool checkButtonPress(int pin, bool &lastReading, unsigned long &lastPressTime) {
  bool reading = digitalRead(pin);
  bool pressed = false;

  if (reading == LOW && lastReading == HIGH && (millis() - lastPressTime) > DEBOUNCE_MS) {
    pressed = true;
    lastPressTime = millis();
  }

  lastReading = reading;
  return pressed;
}

// --- Screens ---

void drawMainMenu() {
  tft.fillScreen(TFT_BLACK);
  updateHeader(sdPercentUsed, sdUsedGB, sdTotalGB);
  updateFooter(1); // Highlights 'MIDDLE'
  drawMiddleButtons(menuSelection);
}

void drawTakePhotoScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("IR LEVEL: 12%", 10, 40, 2); // TODO: wire up real IR logic later

  tft.setTextSize(1);
  tft.drawCentreString("PRESS SHUTTER", 80, 110, 2);
}

void drawViewPhotosScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString("SHUTTER = RETURN", 80, 110, 2);
}

// --- Placeholder: implement this yourself ---
void takephoto() {
  // Your photo capture logic goes here.
  // loop() will wait here until this function returns,
  // then it automatically drops back to the main menu.
}

// --- Helper Functions ---

void updateHeader(float percent, float used, float total) {
  header.createSprite(160, 20);
  header.fillSprite(TFT_BLACK);

  // Progress bar background and fill
  header.drawRect(0, 0, 160, 20, TFT_WHITE);
  header.fillRect(1, 1, (int)(percent * 1.58), 18, TFT_BLUE);

  // Text
  header.setTextColor(TFT_WHITE);
  header.setTextSize(1);
  header.drawRightString("SD: " + String((int)percent) + "% FULL", 158, 4, 2);

  header.pushSprite(0, 0);
  header.deleteSprite();
}

void updateFooter(int activeIdx) {
  footer.createSprite(160, 25);
  footer.fillSprite(TFT_BLACK);
  footer.setTextColor(TFT_WHITE);

  int w = 53; // width per segment
  for (int i = 0; i < 3; i++) {
    uint16_t color = (i == activeIdx) ? TFT_BLUE : TFT_BLACK;
    footer.fillRect(i * w, 0, w - 2, 25, color);
    footer.drawRect(i * w, 0, w - 2, 25, TFT_WHITE);
  }

  footer.drawCentreString("L", 26, 6, 2);
  footer.drawCentreString("M", 79, 6, 2);
  footer.drawCentreString("R", 132, 6, 2);

  footer.pushSprite(0, 103);
  footer.deleteSprite();
}

void drawMiddleButtons(int selected) {
  // Take Photo Button
  btnCam.createSprite(140, 35);
  btnCam.fillSprite(TFT_BLACK);
  btnCam.drawRect(0, 0, 140, 35, (selected == 0) ? TFT_BLUE : TFT_WHITE);
  btnCam.pushImage(10, 9, 16, 16, camera_icon_array);
  btnCam.drawString("TAKE PHOTO", 35, 10, 2);
  if (selected == 0) {
    btnCam.fillCircle(128, 8, 4, TFT_BLUE); // selection dot
  }
  btnCam.pushSprite(10, 30);
  btnCam.deleteSprite();

  // View Photos Button
  btnView.createSprite(140, 35);
  btnView.fillSprite(TFT_BLACK);
  btnView.drawRect(0, 0, 140, 35, (selected == 1) ? TFT_BLUE : TFT_WHITE);
  btnView.pushImage(10, 9, 16, 16, view_icon_array);
  btnView.drawString("VIEW PHOTOS", 35, 10, 2);
  if (selected == 1) {
    btnView.fillCircle(128, 8, 4, TFT_BLUE); // selection dot
  }
  btnView.pushSprite(10, 70);
  btnView.deleteSprite();

}

// Call once at boot (and optionally after writing a new photo).
// NOTE: recursive file walk is SLOW on cards with lots of files.
// Do NOT call this every loop().
bool updateSDCardStats() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  hspi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, hspi, 1000000)) {
    Serial.println("SD init failed - check wiring/card");
    sdCardReady = false;
    return false;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card detected");
    sdCardReady = false;
    return false;
  }

  uint64_t totalBytes = SD.cardSize();

  File root = SD.open("/");
  uint64_t usedBytes = getUsedBytes(root);
  root.close();

  sdTotalGB = totalBytes / (1024.0 * 1024.0 * 1024.0);
  sdUsedGB = usedBytes / (1024.0 * 1024.0 * 1024.0);
  sdPercentUsed = (totalBytes > 0) ? ((float)usedBytes / (float)totalBytes) * 100.0 : 0;

  sdCardReady = true;

  Serial.print("SD Total: "); Serial.print(sdTotalGB, 2); Serial.println(" GB");
  Serial.print("SD Used: "); Serial.print(sdUsedGB, 2); Serial.println(" GB");
  Serial.print("SD Used %: "); Serial.println(sdPercentUsed, 1);

  return true;
}