// Blynk Configuration
#define BLYNK_TEMPLATE_ID "YourTemplateID"
#define BLYNK_DEVICE_NAME "PonchoAssist"
#define BLYNK_AUTH_TOKEN "YourAuthToken"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Servo.h>

// WiFi credentials
char ssid[] = "YourWiFi";
char pass[] = "YourPassword";

// Pins
const int trigPin = 2;
const int echoPin = 3;
const int irPin = A0;
const int forcePin = A1;
const int actuatorPin = 13;

Servo servo1, servo2;

// Blynk virtual pins
#define VPIN_SERVO  V0
#define VPIN_ACTUATOR V1

void setup() {
  // Serial + Blynk + WiFi
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(irPin, INPUT);
  pinMode(forcePin, INPUT);
  pinMode(actuatorPin, OUTPUT);

  // Servos
  servo1.attach(5);
  servo2.attach(6);
}

// Sensor and local control
void runSensorLogic() {
  // Ultrasonic distance
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10); digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  int irValue = analogRead(irPin);
  int forceValue = analogRead(forcePin);

  Serial.print("Distance: "); Serial.println(distance);
  Serial.print("IR: "); Serial.println(irValue);
  Serial.print("Force: "); Serial.println(forceValue);

  // Autonomous logic
  if (distance < 10 || forceValue > 700) {
    servo1.write(0); servo2.write(180);             // Defensive posture
    digitalWrite(actuatorPin, HIGH);                // Extend
  } else {
    servo1.write(90); servo2.write(90);             // Neutral
    digitalWrite(actuatorPin, LOW);                 // Retract
  }
}

// Blynk remote overrides 
BLYNK_WRITE(VPIN_SERVO) {
  int val = param.asInt();
  Serial.println(val ? "Voice: Activate Servos" : "Voice: Reset Servos");
  servo1.write(val ? 180 : 0);
  servo2.write(val ? 0 : 180);
}

BLYNK_WRITE(VPIN_ACTUATOR) {
  int val = param.asInt();
  Serial.println(val ? "Voice: Extend Actuator" : "Voice: Retract Actuator");
  digitalWrite(actuatorPin, val);
}

void loop() {
  Blynk.run();
  runSensorLogic();
  delay(500);
}