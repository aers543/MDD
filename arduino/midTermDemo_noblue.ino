#include "DHT.h"
#include "LiquidCrystal.h"
#include "SoftwareSerial.h"

#define DHTPIN1 13
#define DHTPIN2 11
#define DHTTYPE DHT11   // Type of DHT sensor
#define GREEN_LED1 12
#define GREEN_LED2 10
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

SoftwareSerial bluetooth(8, 9);

// Data filtering parameters
float previousFilteredT1 = 0;
float alphaT1 = 0.2; // Smoothing factor (0 < alpha < 1) for temperature 1

float previousFilteredT2 = 0;
float alphaT2 = 0.2; // Smoothing factor (0 < alpha < 1) for temperature 2

const int redTLED1 = A0;
const int yellowHLED1 = A1;
const int redTLED2 = A2;
const int yellowHLED2 = A3;

const float temperatureThreshold = 20.0; // Set your temperature threshold here (in Celsius)
const int humidityThreshold = 80;


void setup() {
  pinMode(GREEN_LED1, OUTPUT);
  pinMode(GREEN_LED2, OUTPUT);
  Serial.begin(9600);          // Initialize serial communication for debugging
  dht1.begin();                // Initialize the DHT sensor
  dht2.begin();
  lcd.begin(16, 2);
  bluetooth.begin(9600);       // Initialize Bluetooth communication
  pinMode(redTLED1, OUTPUT);
  pinMode(yellowHLED1, OUTPUT);
  pinMode(redTLED2, OUTPUT);
  pinMode(yellowHLED2, OUTPUT);
}

void loop() {

  float h1 = dht1.readHumidity();         // Read humidity from DHT11
  float rawT1 = dht1.readTemperature();  // Read temperature from DHT11 in Celsius
  float h2 = dht2.readHumidity();
  float rawT2 = dht2.readTemperature();

  if (  isnan(h1) || isnan(rawT1) ) {
    Serial.println("Failed to read from first sensor");     //don't change
    digitalWrite(GREEN_LED1, LOW);
  }
  else if ( isnan(h2) || isnan(rawT2) ) {
    Serial.println("Failed to read from second sensor!");
    digitalWrite(GREEN_LED2, LOW);
  } 
  
  else {
    // Apply data filtering
    float t1 = alphaT1 * rawT1 + (1 - alphaT1) * previousFilteredT1;
    previousFilteredT1 = t1;

    float t2 = alphaT2 * rawT2 + (1 - alphaT2) * previousFilteredT2;
    previousFilteredT2 = t2;

    digitalWrite(GREEN_LED1, HIGH);
    digitalWrite(GREEN_LED2, HIGH);

    // Print data from first sensor to the serial monitor for debugging
    Serial.print(F("Temperature1: "));
    Serial.print(t1);
    Serial.print(F("°C "));
    Serial.print("\n");

    // Update the LCD with data from the first sensor
    lcd.print("Temp1: ");
    lcd.print(t1);
    lcd.print((char)223);

    lcd.setCursor(0, 1);

    // Send data to the Bluetooth module
    bluetooth.print("\n");
    bluetooth.print("Temperature1: ");
    bluetooth.print(t1);
    bluetooth.println(" °C");

    Serial.print(F("Temperature2: "));
    Serial.print(t2);
    Serial.print(F("°C "));
    Serial.print("\n");

    lcd.print("Temp2: ");
    lcd.print(t2);
    lcd.print((char)223);

    bluetooth.print("Temperature2: ");
    bluetooth.print(t2);
    bluetooth.println(" °C");

    delay(2000);
    lcd.clear();

    Serial.print(F("Humidity1: ")); 
    Serial.print(h1);
    Serial.print("\n");

    lcd.print("Hum1:  ");
    lcd.print(h1); 
    lcd.print("%");

    bluetooth.print("Humidity1: ");
    bluetooth.print(h1);
    bluetooth.print(" %");
    bluetooth.print("\n");

    lcd.setCursor(0, 1);

    Serial.print(F("Humidity2: ")); 
    Serial.print(h2);
    Serial.print("\n");

    lcd.print("Hum2:  ");
    lcd.print(h2);
    lcd.print("%");

    bluetooth.print("Humidity2: ");
    bluetooth.print(h2);
    bluetooth.print(" %");
    bluetooth.print("\n");

    delay(2000);
    lcd.clear();

    if (t1 > temperatureThreshold) {
      digitalWrite(redTLED1, HIGH);   //to light up the red LED if the temperature is above threshold value
    }
    else {    
      digitalWrite(redTLED1, LOW);
    }

    if (h1 > humidityThreshold) {
     digitalWrite(yellowHLED1, HIGH); //to light up the yellow LED if the humidity is above threshold value
    }
    else {
      digitalWrite(yellowHLED1, LOW);
    } 

     if (t2 > temperatureThreshold) {
      digitalWrite(redTLED2, HIGH); 
    }
    else {    
      digitalWrite(redTLED2, LOW);
    }

    if (h2 > humidityThreshold) {
     digitalWrite(yellowHLED2, HIGH);
    }
    else {
      digitalWrite(yellowHLED2, LOW);
    } 
   
  }
    return;
  }
