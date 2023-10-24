#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <HTTPClient.h>

Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

const int fsrPin = 36;          // FSR402 analog pin
int value;
float voltage;
unsigned long resistance = 0;
unsigned long conductance = 0;
float force = 0;

// Data filtering parameters
float previousFilteredT1 = 0;
float alphaT1 = 0.2; // Smoothing factor (0 < alpha < 1) for temperature 1

const int green_bmp = 2;
const int green_pressure = 4;
const int yellow_temp = 19;
const int red_pressure = 5;

const char* ssid = "pohchin_";     // Replace with your Wi-Fi network name
const char* password = "#LOLOLOL"; // Replace with your Wi-Fi network password
String serverHost = "172.20.10.2"; // Replace with your server's IP address
const int serverPort = 3000; // Replace with the server's port if different
const String serverPath = "/data"; // Correct path on your server

bool wifiConnected = false; // Variable to track Wi-Fi connection status

void setup() {
  Serial.begin(115200);
  pinMode(green_bmp, OUTPUT);
  pinMode(green_pressure, OUTPUT);

  pinMode(yellow_temp, OUTPUT);
  pinMode(red_pressure, OUTPUT);

 // Initialize Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");

  wifiConnected = true; // Assume Wi-Fi is connected

  analogReadResolution(12); // Set the resolution to 12 bits (0-4095)
  analogSetAttenuation(ADC_11db); // Set the reference to 3.3V

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
  int voltage = map(fsrValue, 0 , 2464 , 0 , 5000); //map analog value to obtain V0 in mV

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
    Serial.println("pressure not detected");
  } else {
    digitalWrite(green_pressure, HIGH); // Turn on the pressure alert LED
    Serial.println("Analog:");
    Serial.print(fsrValue);
    Serial.println("\n");

    resistance = (50e6 / voltage ) - 10000;
    Serial.println("Voltage:"); 
    Serial.print(voltage);
    Serial.println("\n");
    Serial.print(resistance); 
    Serial.println("NEXT C"); 
 
    conductance = 1000000 / resistance; // in microhms
    Serial.print(conductance); 

    if (conductance <= 1000) {
      force = ( conductance / 80 ) / 9.81;
      Serial.print(force); 
    } else {
      force = ((conductance - 1000) / 30) / 9.81;
      Serial.print(force); 

    }
    if (force > 4.1) {
    digitalWrite(red_pressure,HIGH);
    Serial.print(force); 
  } else {
    digitalWrite(red_pressure,LOW);
    Serial.print(force);
  }
  }
  Serial.print("\n");
  delay(2000);
  
  Serial.print("\n");

  // Send data to the server if Wi-Fi is connected
  if (wifiConnected) {
    sendDataToServer(temperature, fsrValue);
  }

  delay(2000);
}

void sendDataToServer(float temperature, float conductance) {
  HTTPClient http;
  
  String url = "http://" + String(serverHost) + String(serverPath);
  url += "?temp=" + String(temperature, 2);
  url += "&press=" + String(conductance);
  
  Serial.print("Sending data to server: ");
  Serial.println(url);
  
  http.begin(url);
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.print("Error sending data. HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

