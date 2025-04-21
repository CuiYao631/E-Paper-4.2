#define BATTERY_PIN 39      // 更改为 GPIO 35（ADC1_CH7），这是模拟专用引脚，没有内部上拉电阻
#define BATTERY_READ_INTERVAL 60000  // 电池电量读取间隔，单位毫秒（1分钟）
#define BATTERY_MAX_VOLTAGE 4.2      // 电池最高电压（满电）
#define BATTERY_MIN_VOLTAGE 3.3      // 电池最低电压（需要充电）
#define BATTERY_SAMPLES 20           // 增加采样次数提高精度
#define REFERENCE_VOLTAGE 3.3        // ESP32 ADC参考电压
#define VOLTAGE_DIVIDER_RATIO 2.0    // 分压电阻比例（根据实际电路调整）

int batteryLevel = 0;                // 电池电量百分比（0-100）
float batteryVoltage = 0.0;          // 电池电压
unsigned long previousBatteryReadMillis = 0; // 上次读取电池电量的时间

// 初始化电池检测
void initBattery() {
  pinMode(BATTERY_PIN, INPUT); // 配置引脚为输入模式
  
  // 提前配置ADC
  analogReadResolution(12);    // 设置ADC分辨率为12位（0-4095）
  analogSetWidth(12);          // 同样设置为12位宽度
  analogSetAttenuation(ADC_11db); // 设置衰减为11db，增加测量范围
  
  // 进行多次空读来稳定ADC（这在一些ESP32设备上很重要）
  for(int i = 0; i < 5; i++) {
    analogRead(BATTERY_PIN);
    delay(10);
  }
  
  readBatteryLevel();          // 初始读取一次电池电量
  previousBatteryReadMillis = millis(); // 更新上次读取时间
  Serial.println("电池监测初始化完成");
}

// 读取电池电量
void readBatteryLevel() {
  
  // ESP32的ADC读取值范围为0-4095，对应0V-3.3V
  
  // 先丢弃前几个读数（ESP32 ADC稳定化）
  for (int i = 0; i < 3; i++) {
    analogRead(BATTERY_PIN);
    delay(5);
  }
  
  // 读取多次取平均值，提高准确性
  uint32_t rawValue = 0;
  int validReadings = 0;
  int maxReading = 0;
  int minReading = 4095;
  
  for (int i = 0; i < BATTERY_SAMPLES; i++) {
    int reading = analogRead(BATTERY_PIN);
    
    // 记录最大和最小值，用于滤除异常值
    if (reading > maxReading) maxReading = reading;
    if (reading > 0 && reading < minReading) minReading = reading;
    
    if (reading > 0) {  // 只计算有效值
      rawValue += reading;
      validReadings++;
    }
    delay(10);  // 增加延时，让ADC稳定
  }
  
  // 计算平均值（去除最大值和最小值）
  if (validReadings > 2) {
    rawValue = (rawValue - maxReading - minReading) / (validReadings - 2);
  } else if (validReadings > 0) {
    rawValue = rawValue / validReadings;
  } else {
    rawValue = 0;
  }
  
  // ESP32 ADC非线性校正（简化版）
  // 参考: https://github.com/G6EJD/ESP32-ADC-Accuracy-Improvement
  float correctedValue = rawValue;
  if (rawValue < 1 || rawValue > 4095) {
    // 无效读数，保持0
    correctedValue = 0;
  }
  
  // 计算电压值，考虑ESP32 ADC的非线性特性
  batteryVoltage = (correctedValue / 4095.0) * REFERENCE_VOLTAGE * VOLTAGE_DIVIDER_RATIO;
  
  // 计算电池百分比
  if (batteryVoltage >= BATTERY_MAX_VOLTAGE) {
    batteryLevel = 100;
  } else if (batteryVoltage <= BATTERY_MIN_VOLTAGE) {
    batteryLevel = 0;
  } else {
    // 线性映射电压值到百分比
    batteryLevel = (int)((batteryVoltage - BATTERY_MIN_VOLTAGE) / (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) * 100.0);
    // 确保百分比在有效范围内
    if (batteryLevel < 0) batteryLevel = 0;
    if (batteryLevel > 100) batteryLevel = 100;
  }
  
  // 打印详细的调试信息
  Serial.print("电池原始值: ");
  Serial.print(rawValue);
  Serial.print(", 有效读数: ");
  Serial.print(validReadings);
  Serial.print(", 电压: ");
  Serial.print(batteryVoltage, 3); // 显示三位小数
  Serial.print("V, 电量: ");
  Serial.print(batteryLevel);
  Serial.println("%");
}

// 更新电池状态（在主循环中定期调用）
void updateBatteryStatus(bool forceUpdate = false) {
  unsigned long currentMillis = millis();
  
  // 检查是否需要读取电池电量（每分钟一次）或强制更新
  if (forceUpdate || (currentMillis - previousBatteryReadMillis >= BATTERY_READ_INTERVAL)) {
    // 临时关闭WiFi以减少干扰
    bool wasConnected = (WiFi.status() == WL_CONNECTED);
    if (wasConnected) {
      WiFi.disconnect(false);  // 断开WiFi但保持配置
      delay(50);  // 等待WiFi停止活动
    }
    
    // 进行电池读取
    readBatteryLevel();
    previousBatteryReadMillis = currentMillis;
    
    // 如果之前WiFi已连接，则重新连接
    if (wasConnected) {
      WiFi.reconnect();
    }
  } else {
    // 如果没有读取新值，打印当前缓存值用于调试
    Serial.print("电池缓存值 - 原始值: (已缓存), 电压: ");
    Serial.print(batteryVoltage, 3);
    Serial.print("V, 电量: ");
    Serial.print(batteryLevel);
    Serial.println("%");
  }
}

// 获取电池电量图标字符串
String getBatteryIcon() {
  if (batteryLevel >= 80) {
    return "🔋"; // 电池满
  } else if (batteryLevel >= 60) {
    return "🔋"; // 电池高
  } else if (batteryLevel >= 40) {
    return "🔋"; // 电池中
  } else if (batteryLevel >= 20) {
    return "🔋"; // 电池低
  } else {
    return "🪫"; // 电池很低
  }
}

// 获取电池电量百分比
int getBatteryPercentage() {
  return batteryLevel;
}

// 获取电池电压
float getBatteryVoltage() {
  return batteryVoltage;
}

// 在屏幕上显示电池状态
void displayBatteryStatus(int x, int y) {
  // 以文本形式显示电池电量
  String batteryText = String(batteryLevel) + "%";
  u8g2Fonts.setCursor(x, y);
  u8g2Fonts.print(batteryText);
}
