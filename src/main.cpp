// #include <Arduino.h>

// int adcPin = 32;   // ADC 引脚选择 GPIO32
// int adcValue = 0;
// float voltage = 0.0;

// void setup() {
//   Serial.begin(115200);
//   analogSetAttenuation(ADC_11db);  
//   // 设置衰减 11dB，量程大约 0 ~ 3.6V 
//   // 默认是 0dB，量程 0 ~ 1.1V
// }

// void loop() {
//   adcValue = analogRead(adcPin);          // 读取原始值 (0 ~ 4095)
//   voltage  = (adcValue / 4095.0) * 3.3;   // 转换成电压 (假设供电是 3.3V)

//   Serial.print("ADC Value: ");
//   Serial.print(adcValue);
//   Serial.print("  Voltage: ");
//   Serial.println(voltage, 3);  // 保留3位小数

//   delay(1000);
// }
#include <Arduino.h>

#define LEFT_PIN   32
#define RIGHT_PIN  33
#define THRESHOLD  0.05   // 电压阈值
#define VREF       3.3    // 基准电压

int leftValue = 0, rightValue = 0;
float leftVolt = 0.0, rightVolt = 0.0;

bool leftBlockedPrev = false;
bool rightBlockedPrev = false;

int leftBlockCount = 0;
int rightBlockCount = 0;
int totalCount = 0;

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db); // 0~3.6V

  Serial.println("System Ready. Send 'CLR' to reset count.");
}

void loop() {
  // 读取 ADC
  leftValue  = analogRead(LEFT_PIN);
  rightValue = analogRead(RIGHT_PIN);

  leftVolt  = (leftValue  / 4095.0) * VREF;
  rightVolt = (rightValue / 4095.0) * VREF;

  // 检测左侧
  bool leftBlocked = (leftVolt < THRESHOLD);
  if (leftBlocked && !leftBlockedPrev) {
    leftBlockCount++;
    Serial.println("Left Block Detected");
  }
  leftBlockedPrev = leftBlocked;

  // 检测右侧
  bool rightBlocked = (rightVolt < THRESHOLD);
  if (rightBlocked && !rightBlockedPrev) {
    rightBlockCount++;
    Serial.println("Right Block Detected");
  }
  rightBlockedPrev = rightBlocked;

  // 每边都遮挡2次，算一次完整通过
  if (leftBlockCount >= 2 && rightBlockCount >= 2) {
    totalCount++;
    leftBlockCount = 0;
    rightBlockCount = 0;
    Serial.print("Total Count: ");
    Serial.println(totalCount);
  }

  // 串口接收清零指令
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.equalsIgnoreCase("CLR")) {
      totalCount = 0;
      leftBlockCount = 0;
      rightBlockCount = 0;
      Serial.println("Count Cleared.");
    }
  }

  delay(50); // 采样间隔
}
