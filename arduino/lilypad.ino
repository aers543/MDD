#include <OneWire.h>

// Data pin may be different depending on where it is connected to, change the value if required
#define ONE_WIRE_BUS 1

OneWire oneWire(ONE_WIRE_BUS);

void setup() {
  Serial.begin(9600);
}

void loop() {
  float temperatureCelsius;
  byte data[9];

  // Writing down temperature from lilypad sensor
  oneWire.reset();
  oneWire.write(0xCC);
  oneWire.write(0x44);

  // 750ms for 12-bit resolution
  delay(750);

  // Writing down temperature from lilypad sensor
  oneWire.reset();
  oneWire.write(0xCC);
  oneWire.write(0xBE);

  // Read the data from the sensor
  for (int i = 0; i < 9; i++) {
    data[i] = oneWire.read();
  }

  // Calculate temperature in Celsius
  int16_t rawTemperature = (data[1] << 8) | data[0];
  temperatureCelsius = (float)rawTemperature / 16.0;

  // Print the temperature to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureCelsius);
  Serial.println(" °C");

  delay(2000); // Adjust the delay time as needed
}
