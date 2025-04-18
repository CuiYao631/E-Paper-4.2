#define BATTERY_PIN 25      // 电池电量检测引脚
#define BATTERY_READ_INTERVAL 60000  // 电池电量读取间隔，单位毫秒（1分钟）
#define BATTERY_MAX_VOLTAGE 4.2      // 电池最高电压（满电）
#define BATTERY_MIN_VOLTAGE 3.3      // 电池最低电压（需要充电）

int batteryLevel = 0;                // 电池电量百分比（0-100）
float batteryVoltage = 0.0;          // 电池电压
unsigned long previousBatteryReadMillis = 0; // 上次读取电池电量的时间

// 初始化电池检测
void initBattery() {
  pinMode(BATTERY_PIN, INPUT); // 配置引脚为输入模式
  readBatteryLevel();          // 初始读取一次电池电量
  Serial.println("电池监测初始化完成");
}

// 读取电池电量
void readBatteryLevel() {
  // ESP32的ADC读取值范围为0-4095，对应0V-3.3V
  // 假设使用分压电阻，需要根据实际电路调整计算公式
  
  // 读取多次取平均值，提高准确性
  int rawValue = 0;
  for (int i = 0; i < 10; i++) {
    rawValue += analogRead(BATTERY_PIN);
    delay(10);
  }
  rawValue /= 10;
  
  // 转换为电压值 (ESP32 ADC参考电压为3.3V)
  // 如果有分压电路，需要根据分压比例调整
  // 假设使用的是1:1分压电阻，实际值需要乘以2
  batteryVoltage = (rawValue / 4095.0) * 3.3 * 2.0;
  
  // 计算电池百分比
  if (batteryVoltage >= BATTERY_MAX_VOLTAGE) {
    batteryLevel = 100;
  } else if (batteryVoltage <= BATTERY_MIN_VOLTAGE) {
    batteryLevel = 0;
  } else {
    // 线性映射电压值到百分比
    batteryLevel = (int)((batteryVoltage - BATTERY_MIN_VOLTAGE) / (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) * 100.0);
  }
  
  Serial.print("电池原始值: ");
  Serial.print(rawValue);
  Serial.print(", 电压: ");
  Serial.print(batteryVoltage);
  Serial.print("V, 电量: ");
  Serial.print(batteryLevel);
  Serial.println("%");
}

// 更新电池状态（在主循环中定期调用）
void updateBatteryStatus() {
  unsigned long currentMillis = millis();
  
  // 检查是否需要读取电池电量（每分钟一次）
  if (currentMillis - previousBatteryReadMillis >= BATTERY_READ_INTERVAL) {
    readBatteryLevel();
    previousBatteryReadMillis = currentMillis;
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
