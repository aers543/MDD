#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

Adafruit_BMP280 bmp; // I2C

const int fsrPin = 32; // FSR402 analog pin
float value;
float voltage;
unsigned long resistance = 0;
unsigned long conductance = 0;
float pressure = 0; // Pressure in N/cm^2

// Data filtering parameters
float previousFilteredT1 = 0;
float alphaT1 = 0.2; // Smoothing factor (0 < alpha < 1) for temperature 1

const int bluePin = 2;   // Blue LED pin
const int greenPin = 0;  // Green LED pin
const int redPin = 4;    // Red LED pin

const char* ssid = "pohchin_";     // Replace with your Wi-Fi network name
const char* password = "#LOLOLOL"; // Replace with your Wi-Fi network password
String serverHost = "172.20.10.2"; // Replace with your server's IP address
const int serverPort = 3000;       // Replace with the server's port if different
const String serverPath = "/data";  // Correct path on your server

bool wifiConnected = false; // Variable to track Wi-Fi connection status

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");

  wifiConnected = true; // Assume Wi-Fi is connected

  analogReadResolution(12); // Set the resolution to 12 bits (0-4095)
  analogSetAttenuation(ADC_11db); // Set the reference to 3.3V

  Serial.begin(115200);
  delay(1000);  
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }


  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

// Calibration data points
int fsrValues[] = {0, 150, 1100, 1450, 1750, 2100, 2350, 2400, 2420, 2480, 2490}; // FSR values
float forceValues[] = {15.0, 20.0, 30.0, 40.0 , 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 110.0}; // Corresponding forces in grams

float convertToPressure(int fsrValue) {
  if (fsrValue <= fsrValues[0]) {
    return 0; // Below the calibration range
  } else if (fsrValue >= fsrValues[10]) {
    // Convert force in grams to force in N, then to pressure in N/cm^2
    return (forceValues[10] / 9.0) / (PI * pow(0.9, 2));
  } else {
    // Linear interpolation
    for (int i = 1; i < 11; i++) {
      if (fsrValue <= fsrValues[i]) {
        // Convert interpolated force in grams to force in N, then to pressure in N/cm^2
        float force = forceValues[i - 1] + (fsrValue - fsrValues[i - 1]) * (forceValues[i] - forceValues[i - 1]) / (fsrValues[i] - fsrValues[i - 1]);
        return (force / 9.0) / (PI * pow(0.9, 2));
      }
    }
  }
  return 0; // Default case
}

void loop() {
  
  float temperature = bmp.readTemperature();

  int fsrValue = analogRead(fsrPin);
  pressure = convertToPressure(fsrValue); // Calculate pressure using the calibration


  float filteredT1 = (alphaT1 * temperature) + (1 - alphaT1) * previousFilteredT1; // Apply smoothing to temperature data
  previousFilteredT1 = filteredT1;

//  unsigned long resistance = (50e6 / voltage) - 10000;
//  Serial.print("Voltage:");
//  Serial.println(voltage);
//  Serial.print("Resistance:");
//  Serial.println(resistance);
//  unsigned long conductance = 1000000 / resistance; // in microhms
//  Serial.print("Conductance:");
//  Serial.println(conductance);

//  if (conductance <= 1000) {
//    force = ((conductance / 80) / 9.81);
//  } else {
//    force = (((conductance - 1000) / 30) / 9.81);
//  }
Serial.print("FSR Value: ");
Serial.println(fsrValue);
Serial.print(F("Calculated Pressure: "));
Serial.print(pressure);
Serial.println(" N/cm^2");
Serial.print(F("Temperature = "));
Serial.print(temperature);
Serial.println(" *C");

  // Condition 1: Both BMP280 and FSR402 below threshold
  if (temperature < 27 && pressure < 3.5) {
    setRGBColor(0, 180, 0); // Green
//    Serial.print(F("Temperature = "));
//    Serial.print(temperature);
//    Serial.println(" *C");
//    Serial.print("Analog:");
//    Serial.println(fsrValue);
//    Serial.print("Normal force:");
//    Serial.println(force);
  }

  // Condition 2: BMP280 temperature above 20
  else if (temperature > 27 && pressure < 3.5) {
    setRGBColor(0, 0, 255); // BLUE
    Serial.print("bad temperature");
    Serial.print(temperature);
    Serial.println(" *C");
//    Serial.print("Analog:");
//    Serial.println(fsrValue);
  //  Serial.print("Normal force:");
  //  Serial.println(force);
  }

  // Condition 3: FSR402 force above 4.1
  else if (temperature < 27 && pressure > 3.5) {
    setRGBColor(60, 0, 60); // Purple
//    Serial.print(F("Temperature = "));
//    Serial.print(temperature);
//    Serial.println(" *C");
  //  Serial.print("Analog:");
  //  Serial.println(fsrValue);
    Serial.println("bad force");
  //  Serial.print("HIGH force:");
  //  Serial.println(force);
  }

  // Condition 4: BMP280 temperature above 20 and FSR402 force above 4.1
  else if (temperature > 27 && pressure > 3.5) {
    setRGBColor(255, 0, 0); // Red
    Serial.print("bad temperature");
//    Serial.print(temperature);
//    Serial.println(" *C");
  //  Serial.print("Analog:");
  //  Serial.println(fsrValue);
    Serial.println("bad force");
   // Serial.print("HIGH force:");
  //  Serial.println(force);
  }

  Serial.print("\n");
//  delay(2000);
//  Serial.print("\n");

  // Send data to the server if Wi-Fi is connected
  if (wifiConnected) {
    sendDataToServer(filteredT1, "temperature");
    sendDataToServer(pressure, "pressure");
  }
  delay(500);
}

void setRGBColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void sendDataToServer(float value, String sensorType) {
  HTTPClient http;

  String url = "http://" + String(serverHost) + ":" + String(serverPort) + String(serverPath);

  // Create a JSON object and populate it
  DynamicJsonDocument jsonDoc(200);
  jsonDoc["sensor_type"] = sensorType;
  jsonDoc["value"] = value;

  // Serialize the JSON object to a string
  String postData;
  serializeJson(jsonDoc, postData);

  Serial.print("Sending data to the server: ");
  Serial.println(url);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(postData); 
  if (httpResponseCode == 200) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.print("Error sending data. HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  delay(1000);
}
