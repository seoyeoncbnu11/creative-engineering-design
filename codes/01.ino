#1
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Servo.h>

// LCD 및 키패드 설정
LiquidCrystal_I2C lcd(0x27, 16, 2);
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// 핀 정의
#define DHTPIN A2
#define DHTTYPE DHT11
#define FLAME_PIN A0
#define LIGHT_PIN A1
#define LED_PIN 13
#define BUZZER_PIN 11
#define SERVO_PIN 12

// 센서 및 액추에이터 객체
DHT dht(DHTPIN, DHTTYPE);
Servo servo;

// 서보 부드럽게 회전용 변수
int servoAngle = 0;
int servoStep = 25;

// 메뉴 상태
enum MenuState {
  MAIN_MENU,
  DRINK_MENU,
  FOOD_MENU,
  CHECKOUT
};
MenuState currentMenu = MAIN_MENU;

String inputBuffer = "";

// 메뉴 UI 함수들
void showMainMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1:drink 2:food");
  lcd.setCursor(0, 1);
  lcd.print("3:pay *:cancel");
}

void showDrinkMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("select drink");
  lcd.setCursor(0, 1);
  lcd.print("input quantity:#complete");
}

void showFoodMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("select food");
  lcd.setCursor(0, 1);
  lcd.print("input quantity:#complete");
}

void showCheckout() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("paying...");
  delay(1500);
  lcd.clear();
  lcd.print("thank you!");
  delay(2000);
  showMainMenu();
  currentMenu = MAIN_MENU;
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FLAME_PIN, INPUT);

  servo.attach(SERVO_PIN);
  servo.write(0);

  showMainMenu();
}

void loop() {
  // 1. 키패드 메뉴 입력 처리
  char key = keypad.getKey();
  if (key) {
    switch (currentMenu) {
      case MAIN_MENU:
        if (key == '1') {
          currentMenu = DRINK_MENU;
          inputBuffer = "";
          showDrinkMenu();
        } else if (key == '2') {
          currentMenu = FOOD_MENU;
          inputBuffer = "";
          showFoodMenu();
        } else if (key == '3') {
          currentMenu = CHECKOUT;
          showCheckout();
        }
        break;

      case DRINK_MENU:
      case FOOD_MENU:
        if (key >= '0' && key <= '9') {
          if (inputBuffer.length() < 10) {
            inputBuffer += key;
            lcd.setCursor(0, 1);
            lcd.print("count: " + inputBuffer + "   ");
          }
        } else if (key == '#') {
          Serial.print("order: ");
          if (currentMenu == DRINK_MENU) Serial.print("drink ");
          else Serial.print("food ");
          Serial.println(inputBuffer);

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("order!");
          delay(1500);
          showMainMenu();
          currentMenu = MAIN_MENU;
          inputBuffer = "";
        } else if (key == '*') {
          showMainMenu();
          currentMenu = MAIN_MENU;
          inputBuffer = "";
        }
        break;

      default:
        break;
    }
  }

  // 2. 센서 데이터 읽기 및 출력
  int lightValue = analogRead(LIGHT_PIN);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int flameValue = analogRead(FLAME_PIN);

  Serial.print("조도: "); Serial.print(lightValue);
  Serial.print(" 온도: "); Serial.print(temp);
  Serial.print(" 습도: "); Serial.print(hum);
  Serial.print(" 불꽃: "); Serial.println(flameValue);

  // 3. LED 제어 (조도 기준)
  if (lightValue > 600) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // 4. 부저 제어 (불꽃 감지 시)
  if (flameValue > 100) {
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
  }

  // 5. 서보 모터 부드럽게 회전 (조건 만족 시)
  if (lightValue > 800 || (temp > 30 || hum > 70)) {
    servo.write(servoAngle);
    servoAngle += servoStep;
    if (servoAngle >= 180 || servoAngle <= 0) {
      servoStep = -servoStep;
    }
  } else {
    servo.write(0);
  }

  delay(100);
}
