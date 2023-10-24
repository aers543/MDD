#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>

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

const char* ssid = "ckinie";     // Replace with your Wi-Fi network name
const char* password = "wifiisdown"; // Replace with your Wi-Fi network password
const char* serverHost = "localhost"; // Ervin replaced with server host details 
const String serverPath = "/data"; // Ervin replaced with coorect path to details

void setup() {
  Serial.begin(115200);
  pinMode(green_bmp, OUTPUT);
  pinMode(green_pressure, OUTPUT);

  pinMode(yellow_temp, OUTPUT);
  pinMode(red_pressure, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


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
  

}

void loop() {
  float temperature = bmp.readTemperature();

  int fsrValue = analogRead(fsrPin);

  float filteredT1 = (alphaT1 * temperature) + (1 - alphaT1) * previousFilteredT1;    // Apply smoothing to temperature data
  previousFilteredT1 = filteredT1;

  // Check if BMP280 and FSR402 are working
  if (isnan(temperature)) {
    digitalWrite(green_bmp, LOW); // Turn OFF the BMP LED to indicate an issue
    Serial.println("Temperature not detected");
   
  } else {
    digitalWrite(green_bmp, HIGH); // Turn ON the BMP LED to indicate it's working
    Serial.print(F("Temperature = "));
    Serial.print(filteredT1);
    Serial.println(" *C");

  if (filteredT1 > 20) {
      digitalWrite(yellow_temp,HIGH);
  } else {    
      digitalWrite(yellow_temp, LOW);
    }

  }

  if (isnan(fsrValue)) {
    digitalWrite(green_pressure, LOW); // Turn OFF the BMP LED to indicate an issue
    Serial.println("temperature not detected");
  } else {
    digitalWrite(green_pressure, HIGH); // Turn on the pressure alert LED

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

void loop2() {
  float temperature = bmp.readTemperature();

  int fsrValue = analogRead(fsrPin);

  float filteredT1 = (alphaT1 * temperature) + (1 - alphaT1) * previousFilteredT1;    // Apply smoothing to temperature data
  previousFilteredT1 = filteredT1;
   // Create an HTTP client object
  WiFiClient client;
 // Construct the URL with temperature and pressure data as parameters
  String url = serverPath + "?temp=" + String(filteredT1, 2) + "&press=" + String(fsrValue, 2);
     // Make an HTTP GET request
  if (client.connect(serverHost, 80)) {
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + serverHost + "\r\n" +
                 "Connection: close\r\n\r\n");
    client.stop();
  } else {
    Serial.println("Connection to server failed");
  }
  delay(60000); // Wait for a minute before sending data again

} 
