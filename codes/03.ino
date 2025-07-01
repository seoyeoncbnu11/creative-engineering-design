#include <Stepper.h>

#define PIR_SENSOR 2
#define SWITCH_PIN 3

#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

const int STEPS = 200;  // 모터 1회전 스텝 수

int personCount = 0;
bool prevMotion = false;

Stepper stepper(STEPS, 5, 7, 4, 6);

void setup() {
  pinMode(PIR_SENSOR, INPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.begin(9600);
  stepper.setSpeed(3);
}

void loop() {
  bool motion = digitalRead(PIR_SENSOR);
  bool switchPressed = digitalRead(SWITCH_PIN) == LOW;

  // 시리얼 모니터로 PIR 상태 확인
  Serial.print("PIR 상태: ");
  Serial.println(motion);

  // 사람 감지: LOW → HIGH 전이 시
  if (motion && !prevMotion) {
    if (personCount < 8) {
      personCount++;
      Serial.print("현재 인원: ");
      Serial.println(personCount);
      Door();
      RGBColor(personCount);
      delay(500); // 중복 감지 방지
    }
  }
  prevMotion = motion;

  // 스위치 눌러서 인원 감소
  if (switchPressed && personCount > 0) {
    personCount--;
    Serial.print("현재 인원: ");
    Serial.println(personCount);
    Door();
    RGBColor(personCount);
    delay(500); // 디바운싱
  }
}

void Door() {
  stepper.step(STEPS / 2);  // 문 열기
  delay(500);
  stepper.step(-STEPS / 2); // 문 닫기
}

void RGBColor(int count) {
  count = constrain(count, 0, 8);
  int redVal = map(count, 0, 8, 0, 255);
  int greenVal = map(count, 0, 8, 255, 0);

  analogWrite(RED_PIN, redVal);
  analogWrite(GREEN_PIN, greenVal);
  analogWrite(BLUE_PIN, 0); // 파란색 미사용
}
