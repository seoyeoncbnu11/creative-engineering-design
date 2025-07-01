#2
int setNum[10][8] = {
  {1, 1, 1, 1, 1, 1, 0, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1, 0}, // 2
  {1, 1, 1, 1, 0, 0, 1, 0}, // 3
  {0, 1, 1, 0, 0, 1, 1, 0}, // 4
  {1, 0, 1, 1, 0, 1, 1, 0}, // 5
  {1, 0, 1, 1, 1, 1, 1, 0}, // 6
  {1, 1, 1, 0, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1, 0}, // 8
  {1, 1, 1, 1, 0, 1, 1, 0}  // 9
};

int segmentPins[7] = {4, 5, 6, 7, 8, 9, 10}; // A~G
#define BUZZER 11
#define SOUND_LED 2
#define SOUND_SENSOR A0
#define WATER_SENSOR A5
#define WATER_LED 3
#define SWITCH1 12
#define SWITCH2 13
#define TILT_SENSOR A2

int prevTiltVal = 0;

void setup() {
  Serial.begin(9600);

  pinMode(SOUND_LED, OUTPUT);
  pinMode(WATER_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  for (int i = 0; i < 7; i++) pinMode(segmentPins[i], OUTPUT);

  pinMode(SOUND_SENSOR, INPUT);
  pinMode(WATER_SENSOR, INPUT);
  pinMode(SWITCH1, INPUT_PULLUP);
  pinMode(SWITCH2, INPUT_PULLUP);
  pinMode(TILT_SENSOR, INPUT);
}

void loop() {
  int soundVal = analogRead(SOUND_SENSOR);
  int delayTime = map(soundVal, 100, 800, 200, 20);
  digitalWrite(SOUND_LED, HIGH); delay(delayTime);
  digitalWrite(SOUND_LED, LOW); delay(delayTime);

  int waterLevel = analogRead(WATER_SENSOR);
  digitalWrite(WATER_LED, (waterLevel < 300 || waterLevel > 700) ? HIGH : LOW);

  int tilt = analogRead(TILT_SENSOR);
  int diff = abs(tilt - prevTiltVal);
  if (diff > 15) {
    tone(BUZZER, 1500, 300);
  }
  prevTiltVal = tilt;

  bool sw1 = digitalRead(SWITCH1) == LOW;
  bool sw2 = digitalRead(SWITCH2) == LOW;

  if (sw1) {
    displayNumber(1);
  } else if (sw2) {
    displayNumber(2);
  } else {
    clearDisplay();
  }

  Serial.print("Sound: ");
  Serial.print(soundVal);
  Serial.print("  Water: ");
  Serial.print(waterLevel);
  Serial.print("  Tilt: ");
  Serial.print(tilt);
  Serial.print("  ΔTilt: ");
  Serial.print(diff);
  Serial.print("  SW1: ");
  Serial.print(sw1 ? "Pressed" : "Released");
  Serial.print("  SW2: ");
  Serial.println(sw2 ? "Pressed" : "Released");
}

// setNum[][] 기반 숫자 표시
void displayNumber(int num) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], setNum[num][i]);
  }
}

// 7세그먼트 끄기 함수
void clearDisplay() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], LOW);
  }
}
