#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

const int fsrPin = 36;          // FSR402 analog pin


// Data filtering parameters
float previousFilteredT1 = 0;
float alphaT1 = 0.2; // Smoothing factor (0 < alpha < 1) for temperature 1

const int green_bmp = 2;
const int green_pressure = 4;
const int yellow_temp = 19;
const int red_pressure = 5;

#define SERVICE_UUID           "0000180a-0000-1000-8000-00805f9b34fb"
#define TEMPERATURE_CHAR_UUID  "00002a6e-0000-1000-8000-00805f9b34fb"
#define PRESSURE_CHAR_UUID     "00002a6d-0000-1000-8000-00805f9b34fb"

BLEServer* pServer;
BLEService* pService;
BLECharacteristic* pTemperatureCharacteristic;
BLECharacteristic* pPressureCharacteristic;

void setup() {
  Serial.begin(115200);
  pinMode(green_bmp, OUTPUT);
  pinMode(green_pressure, OUTPUT);

  pinMode(yellow_temp, OUTPUT);
  pinMode(red_pressure, OUTPUT);

  BLEDevice::init("GR8BAND8"); // Change "YourESP32" to your desired BLE device name


  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pTemperatureCharacteristic = pService->createCharacteristic(
    TEMPERATURE_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pPressureCharacteristic = pService->createCharacteristic(
    PRESSURE_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  
  pService->start();
  BLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {
  float temperature = bmp.readTemperature();
   // Apply smoothing to temperature data

  int fsrValue = analogRead(fsrPin);

  // Check if BMP280 and FSR402 are working
  if (isnan(temperature)) {
    digitalWrite(green_bmp, LOW); // Turn OFF the BMP LED to indicate an issue
    Serial.println("Temperature not detected");
   
  } else {
    digitalWrite(green_bmp, HIGH); // Turn ON the BMP LED to indicate it's working
    float filteredT1 = (alphaT1 * temperature) + (1 - alphaT1) * previousFilteredT1;
    previousFilteredT1 = filteredT1;
    Serial.print(F("Temperature = "));
    Serial.print(filteredT1);
    Serial.println(" *C");
    pTemperatureCharacteristic->setValue(String(filteredT1, 2).c_str());
  
    pTemperatureCharacteristic->notify();

    if (filteredT1 > 20) {
      digitalWrite(yellow_temp,HIGH);
    }
    else {    
      digitalWrite(yellow_temp, LOW);
    }

  }

  if (isnan(fsrValue)) {
    digitalWrite(green_pressure, LOW); // Turn OFF the BMP LED to indicate an issue
    Serial.println("temperature not detected");
  } else {
    digitalWrite(green_pressure, HIGH); // Turn on the pressure alert LED
    pPressureCharacteristic->setValue(String(fsrValue, 2).c_str());
    pPressureCharacteristic->notify();

    if (fsrValue > 1000) {
    digitalWrite(red_pressure,HIGH);
    Serial.print(fsrValue); 
  } else {
    digitalWrite(red_pressure,LOW);
    Serial.print(fsrValue);
  }
  }
    Serial.print("\n");
    delay(2000);
}