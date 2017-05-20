// Test reading the YL-69 sensor using an analog pin

// Temporarily put the A0 soil moisture pin to A15 of
// the Mega 2560
int soilPin = A15;

void setup() {
  pinMode(soilPin,INPUT);
  Serial.begin(9600);
  // Wait for serial port to connect
  while (!Serial) {
  }
  Serial.println("YL-69 Analog Soil Mosture Read Test");
}

void loop() {
  // Higher values are dry; lower values are wet
  int sensorValue = analogRead(soilPin);
  Serial.print("Soil moisture raw value: ");
  Serial.println(sensorValue);
  delay(3000);
}
