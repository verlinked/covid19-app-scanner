/*  

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

  The green tower examples uses an ESP32 as its BLE device and a NeoPixel
  LED circle to indicate the can results.

  The LED circle with rotate while scanning, light green when a nearby
  smartphone with the Corona-Warn-App was detected or light red after
  some timeout scanning without a result.

  Depending on the sensivity of your EPS32 variant you need to adjust
  the APP_DETECTION_RSSI_THRESHOLD which controls how close a smartphone
  has to come on order to be detected. Having this value too low will
  give you detection results from more than one smartphone nearby. This
  may not be want you want.

  You may need to change the LED_CIRCLE_PIN to match your hardware setup.

  Have fun and stay healthy!
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <NeoPixelBrightnessBus.h>

#define APP_DETECTION_RSSI_THRESHOLD -70

#define LED_CIRCLE_PIN 13
#define LED_CIRCLE_LEN 12

#define COVID_SERVICE_UUID 0xFD6F

BLEUUID covidBLEUUID = BLEUUID((uint16_t)COVID_SERVICE_UUID);
BLEScan* covidBLEScan;

uint8_t scanTime = 1; // in seconds
bool scanning = false;
uint8_t scanTimeoutCounter = 0;

bool covidAppNearby = false;
bool covidAppNotFound = false;
uint8_t timeoutCounter = 0;

NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> circle(LED_CIRCLE_LEN, LED_CIRCLE_PIN);

/**
 * Callback when a device was found and advertised.
 */
class BLEDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    bool hasCovidApp = advertisedDevice.isAdvertisingService(covidBLEUUID);
    int rssi = advertisedDevice.getRSSI();
    if (hasCovidApp) {
      if (rssi > APP_DETECTION_RSSI_THRESHOLD) {
        covidAppNearby = true;
      }
    }
  }
};

/**
 * Callback invoked when scanning has completed.
 */
static void scanCompleteCallback(BLEScanResults scanResults) {
  scanning = false;
} 


/**
 * Setup the hardware.
 */
void setup() {
  circle.Begin();
  circle.SetBrightness(200);
  circle.Show();

  BLEDevice::init("");
  covidBLEScan = BLEDevice::getScan(); //create new scan
  covidBLEScan->setAdvertisedDeviceCallbacks(new BLEDeviceCallbacks(), false);
  covidBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  covidBLEScan->setInterval(100);
  covidBLEScan->setWindow(99);  // less or equal setInterval value
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
    scanning = covidBLEScan->start(scanTime, scanCompleteCallback);
  }
  
  updateCircle();
  delay(83);
}
