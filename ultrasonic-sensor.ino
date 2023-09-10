#define LDR_SENSOR_PIN 4
#define TRIGGER_PIN 12
#define ECHO_PIN 8
#define LDR_OUTPUT_PIN 11
#define ULTRASONIC_OUTPUT_PIN 9
#define DISTANCE_THRESHOLD 70
#define ULTRASONIC_DELAY 15000
#define LDR_DELAY_MILLIS 30000 // Converted to milliseconds


unsigned long previousMillis = 0;
unsigned long ultrasonicTimestamp = 0;
unsigned long ldrTimestamp = 0;
int lastLdrValue = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LDR_SENSOR_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LDR_OUTPUT_PIN, OUTPUT);
  pinMode(ULTRASONIC_OUTPUT_PIN, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // Only perform sensor readings every 1000 ms
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    // Read the digital value from the LDR module
    int ldrValue = digitalRead(LDR_SENSOR_PIN);

    // Enable or disable the LDR_OUTPUT_PIN based on the LDR value
    if (ldrValue != lastLdrValue) {
      lastLdrValue = ldrValue;
      ldrTimestamp = currentMillis;
    }
    if (currentMillis - ldrTimestamp > LDR_DELAY_MILLIS) {
      digitalWrite(LDR_OUTPUT_PIN, ldrValue == 1 ? HIGH : LOW);
    }

    // Send a 10-microsecond pulse to the trigger pin to initiate the distance measurement
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(5); // Increased for reliability
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10); // Keep the delay as is
    digitalWrite(TRIGGER_PIN, LOW);

    // Read the time taken for the echo pin to go HIGH, which is proportional to the distance
    long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Set timeout to 30ms

    // Calculate the distance (in cm) based on the speed of sound (343 m/s)
    // Use float division to improve accuracy
    float distance = (duration * 0.0343) / 2.0;

    // Enable or disable the ULTRASONIC_OUTPUT_PIN based on the distance threshold and delay
    if (distance < DISTANCE_THRESHOLD && lastLdrValue == 1) {
      digitalWrite(ULTRASONIC_OUTPUT_PIN, HIGH);
      ultrasonicTimestamp = currentMillis;
    } else if (currentMillis - ultrasonicTimestamp > ULTRASONIC_DELAY) {
      digitalWrite(ULTRASONIC_OUTPUT_PIN, LOW);
    }

    // Print LDR and distance values for debugging purposes
    Serial.print("LDR Value: ");
    Serial.print(ldrValue);
    Serial.print(", Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
}