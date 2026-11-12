
// ==============================
// 腳位設定
// ==============================

const int buttonPin = 2;

const int RledPin = 10;
const int GledPin = 11;
const int BledPin = 9;


// ==============================
// 按鈕設定
// ==============================

// 假設按下 LOW、放開 HIGH
const int PRESSED_STATE = LOW;

const unsigned long debounceDelay = 30;
const unsigned long longPressTime = 3000;


// ==============================
// 模式設定
// ==============================

// 0 = 常亮
// 1 = 閃爍
// 2 = 呼吸燈

int mode = 0;


// ==============================
// 顏色設定
// ==============================

// 紅、綠、藍、黃、紫、青、白

const int colors[7][3] = {
  {255, 0, 0},
  {0, 255, 0},
  {0, 0, 255},
  {255, 255, 0},
  {255, 0, 255},
  {0, 255, 255},
  {255, 255, 255}
};

int colorIndex = 0;


// ==============================
// 按鈕狀態
// ==============================

int lastReading;
int buttonState;

unsigned long lastDebounceTime = 0;
unsigned long pressStartTime = 0;

bool longPressTriggered = false;


// ==============================
// LED 時間設定
// ==============================

unsigned long previousMillis = 0;
unsigned long modeStartMillis = 0;

const unsigned long blinkInterval = 500;

bool ledOn = true;


// ==============================
// RGB LED 控制
// ==============================

// 共陽極 LED，輸出亮度反向

void setLED(int brightness) {

  int r = colors[colorIndex][0] * brightness / 255;
  int g = colors[colorIndex][1] * brightness / 255;
  int b = colors[colorIndex][2] * brightness / 255;

  analogWrite(RledPin, 255 - r);
  analogWrite(GledPin, 255 - g);
  analogWrite(BledPin, 255 - b);
}


// ==============================
// 切換顏色
// ==============================

void changeColor() {

  colorIndex++;

  if (colorIndex >= 7) {
    colorIndex = 0;
  }

  // 常亮模式：立即更新顏色
  if (mode == 0) {
    setLED(255);
  }

  // 閃爍模式：切換顏色後從亮燈開始
  else if (mode == 1) {

    ledOn = true;
    previousMillis = millis();

    setLED(255);
  }
}


// ==============================
// 切換模式
// ==============================

void changeMode() {

  mode++;

  if (mode >= 3) {
    mode = 0;
  }

  unsigned long now = millis();

  previousMillis = now;
  modeStartMillis = now;

  ledOn = true;

  // 進入常亮或閃爍模式
  if (mode == 0 || mode == 1) {
    setLED(255);
  }

  // 進入呼吸燈模式，從最暗開始
  else if (mode == 2) {
    setLED(0);
  }

  Serial.print("Mode: ");
  Serial.println(mode);
}


// ==============================
// 按鈕控制
// ==============================

void checkButton() {

  int reading = digitalRead(buttonPin);

  // 偵測訊號是否改變
  if (reading != lastReading) {

    lastDebounceTime = millis();

    lastReading = reading;
  }

  // 訊號穩定超過防彈跳時間
  if (millis() - lastDebounceTime >= debounceDelay) {

    if (reading != buttonState) {

      buttonState = reading;

      // 按下按鈕
      if (buttonState == PRESSED_STATE) {

        pressStartTime = millis();

        longPressTriggered = false;
      }

      // 放開按鈕
      else {

        // 未達長按時間：切換顏色
        if (!longPressTriggered) {

          changeColor();
        }
      }
    }
  }

  // 長按滿 3 秒，立即切換模式
  if (buttonState == PRESSED_STATE &&
      !longPressTriggered &&
      millis() - pressStartTime >= longPressTime) {

    changeMode();

    // 避免同一次按壓重複觸發
    longPressTriggered = true;
  }
}


// ==============================
// LED 模式控制
// ==============================

void updateLED() {

  unsigned long now = millis();


  // --------------------------
  // 模式 0：常亮
  // --------------------------

  if (mode == 0) {

    setLED(255);
  }


  // --------------------------
  // 模式 1：閃爍
  // --------------------------

  else if (mode == 1) {

    if (now - previousMillis >= blinkInterval) {

      previousMillis = now;

      ledOn = !ledOn;

      if (ledOn) {
        setLED(255);
      }
      else {
        setLED(0);
      }
    }
  }


  // --------------------------
  // 模式 2：呼吸燈
  // --------------------------

  else if (mode == 2) {

    unsigned long phase =
      (now - modeStartMillis) % 2000;

    int brightness;

    // 逐漸變亮
    if (phase < 1000) {

      brightness = phase * 255 / 1000;
    }

    // 逐漸變暗
    else {

      brightness = (2000 - phase) * 255 / 1000;
    }

    setLED(brightness);
  }
}


// ==============================
// 初始化
// ==============================

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);

  Serial.begin(9600);

  // 初始化按鈕狀態，避免開機誤判
  buttonState = digitalRead(buttonPin);
  lastReading = buttonState;

  // 開機紅色常亮
  setLED(255);
}


// ==============================
// 主程式
// ==============================

void loop() {

  checkButton();

  updateLED();

}
