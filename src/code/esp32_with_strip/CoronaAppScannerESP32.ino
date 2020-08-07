Copyright (c) 2020, verlinked GmbH

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <NeoPixelBrightnessBus.h>

#define LED_CIRCLE_PIN 13
#define LED_CIRCLE_LEN 12

#define COVID_SERVICE_UUID 0xFD6F
#define APP_DETECTION_RSSI_THRESHOLD -70

BLEUUID covidBLEUUID = BLEUUID((uint16_t)COVID_SERVICE_UUID);
BLEScan* covidBLEScan;

uint8_t scanTime = 1; // in seconds
bool scanning = false;
uint8_t scanTimeoutCounter = 0;

bool covidAppNearby = false;
bool covidAppNotFound = false;
uint8_t timeoutCounter = 0;

NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> circle(LED_CIRCLE_LEN, LED_CIRCLE_PIN);

class BLEDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    bool hasCovidApp = advertisedDevice.isAdvertisingService(covidBLEUUID);
    int rssi = advertisedDevice.getRSSI();
    if (hasCovidApp) {
      if (rssi > APP_DETECTION_RSSI_THRESHOLD) {
        covidAppNearby = true;
        Serial.println("COVID-19 App Installed");
        Serial.printf("\tCorona App Found at: %s \n", advertisedDevice.getAddress().toString().c_str());
        Serial.printf("\tRSSI: %d \n", rssi);
      } else {
        Serial.println("COVID-19 App too far away");
        Serial.printf("\tCorona App Found at: %s \n", advertisedDevice.getAddress().toString().c_str());
        Serial.printf("\tRSSI: %d \n", rssi);
      }
    }
  }
};

/**
 * Callback invoked when scanning has completed.
 */
static void scanCompleteCallback(BLEScanResults scanResults) {
  Serial.println("Scan complete");
  scanning = false;
} // scanCompleteCB


void setup() {
    
  Serial.begin(115200);

  circle.Begin();
  circle.SetBrightness(200);
  circle.Show();

  BLEDevice::init("");
  covidBLEScan = BLEDevice::getScan(); //create new scan
  covidBLEScan->setAdvertisedDeviceCallbacks(new BLEDeviceCallbacks(), false);
  covidBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  covidBLEScan->setInterval(100);
  covidBLEScan->setWindow(99);  // less or equal setInterval value

  updateStatus("Scanning...");
}

void updateStatus(String msg) {
  Serial.println(msg);
}

uint8_t pos = 0;
void updateCircle() {
  // Turn off
  circle.ClearTo(RgbColor(0,0,0));

  if (covidAppNearby) {
    // All green
    circle.ClearTo(RgbColor(0,150,0));
  } else if (covidAppNotFound) {
    circle.ClearTo(RgbColor(150,0,0));
  } else {
    // Update scanning circle
    pos = pos + 1;
    if (pos > 11) {
      pos = 0;
    }
    circle.SetPixelColor(pos, RgbColor(0,150,200));
  }
  circle.Show();
}

void loop() {
  if (covidAppNearby) {
    scanTimeoutCounter = 0;
    covidAppNotFound = false;
    
    // App found - start a timeout to display the green circle
    timeoutCounter = timeoutCounter + 1;
    if (timeoutCounter > 50) {
      covidAppNearby = false;
      timeoutCounter = 0;
    }
  } else if (covidAppNotFound) {
    scanTimeoutCounter = 0;
    covidAppNearby = false;
    
    // App not found - start a timeout to display the red circle
    timeoutCounter = timeoutCounter + 1;
    if (timeoutCounter > 20) {
      covidAppNotFound = false;
      timeoutCounter = 0;
    }
  } else if (scanning) {
    scanTimeoutCounter = scanTimeoutCounter + 1;
    if (scanTimeoutCounter > 60) {
      if (!covidAppNearby) {
        covidAppNotFound = true;
      }
    }
  } else {
    Serial.println("Start new scan");
    scanning = covidBLEScan->start(scanTime, scanCompleteCallback);
  }
  
  updateCircle();
  delay(83);
}
