/**
 * 传感器管理模块
 * 负责管理SHTC3温湿度传感器
 * 引脚配置在config.h中使用I2C通信(SDA:21, SCL:22)
 */

#include <Wire.h>
#include "config.h"
#include "Adafruit_SHTC3.h"

// 使用config.h中定义的传感器数据显示位置常量

// 函数前向声明
bool initSHTC3(bool readDataAfterInit, bool updateDisplay, uint8_t updateMode);
bool readSHTC3(bool updateDisplay, uint8_t updateMode);
void displayTemperature(float t);
void displayHumidity(float h);
void displaySensorData(float t, float h);
void scanI2C();
void updateSensorData(uint8_t updateMode);

// 创建传感器对象
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

// 传感器数据变量
float temperature = 0.0;    // 温度，单位摄氏度
float humidity = 0.0;       // 湿度，单位百分比

// 传感器状态标志
bool shtc3Initialized = false;   // SHTC3初始化状态

// 上次读取时间
unsigned long lastSensorReadTime = 0;
// 使用config.h中定义的SENSOR_READ_INTERVAL

/**
 * 初始化I2C总线和温湿度传感器
 * 返回值: true=传感器初始化成功，false=传感器初始化失败
 * 参数: readDataAfterInit - 是否在初始化后立即读取数据
 *       updateDisplay - 是否在读取数据后更新显示
 *       updateMode - 更新显示模式(0=全部, 1=仅温度, 2=仅湿度)
 */
bool initSensors(bool readDataAfterInit, bool updateDisplay, uint8_t updateMode) {
  // 初始化I2C总线，添加上拉电阻
  Wire.begin(PIN_SDA, PIN_SCL);
  // 设置较低的时钟频率，提高兼容性
  Wire.setClock(100000); // 100 kHz
  
  // 打印I2C扫描结果，帮助调试
  //scanI2C();
  
  // 初始化温湿度传感器
  return initSHTC3(readDataAfterInit, updateDisplay, updateMode);
}

/**
 * 扫描I2C总线上的设备
 * 用于调试I2C连接问题
 */
void scanI2C() {
  byte error, address;
  int deviceCount = 0;
  
  Serial.println("扫描I2C总线...");
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("发现I2C设备，地址: 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      deviceCount++;
    } else if (error == 4) {
      Serial.print("在地址0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println("出现未知错误");
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("未找到任何I2C设备!");
  } else {
    Serial.print("扫描完成，找到");
    Serial.print(deviceCount);
    Serial.println("个设备");
  }
}

/**
 * 初始化SHTC3温湿度传感器
 * 返回值: true=初始化成功，false=初始化失败
 * 参数: readDataAfterInit - 是否在初始化后立即读取数据
 *       updateDisplay - 是否在读取数据后更新显示
 *       updateMode - 更新显示模式(0=全部, 1=仅温度, 2=仅湿度)
 */
bool initSHTC3(bool readDataAfterInit, bool updateDisplay, uint8_t updateMode) {
  Serial.println("正在初始化SHTC3温湿度传感器...");
  
  if (!shtc3.begin()) {
    Serial.println("找不到SHTC3传感器，请检查连接!");
    shtc3Initialized = false;
    return false;
  }
  
  Serial.println("SHTC3传感器初始化成功!");
  shtc3Initialized = true;
  
  // // 初始化后根据参数决定是否立即读取一次数据
  // if (readDataAfterInit) {
  //   readSHTC3(updateDisplay, updateMode);
  // }
  
  return true;
}

/**
 * 读取SHTC3温湿度数据
 * 返回值: true=读取成功，false=读取失败或传感器未初始化
 * 参数: updateDisplay - 是否更新显示(true=更新, false=不更新)
 *       updateMode - 更新模式(0=全部更新, 1=仅温度, 2=仅湿度)
 */
bool readSHTC3(bool updateDisplay, uint8_t updateMode) {
  if (!shtc3Initialized) {
    return false;
  }
  
  sensors_event_t humidity_event, temp_event;
  
  if (!shtc3.getEvent(&humidity_event, &temp_event)) {
    Serial.println("读取SHTC3数据失败!");
    return false;
  }
  
  // 更新全局变量
  temperature = temp_event.temperature;
  humidity = humidity_event.relative_humidity;
  
  // 打印读取结果
  Serial.print("温度: ");
  Serial.print(temperature);
  Serial.print(" °C, 湿度: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  // 根据参数更新屏幕显示
  if (updateDisplay) {
    if (updateMode == 0) {
      // 同时更新温度和湿度
      displaySensorData(temperature, humidity);
    } else if (updateMode == 1) {
      // 仅更新温度
      displayTemperature(temperature);
    } else if (updateMode == 2) {
      // 仅更新湿度
      displayHumidity(humidity);
    }
  }
  
  return true;
}



/**
 * 定期更新传感器数据
 * 在loop()中定期调用
 * 参数: updateMode - 更新模式(0=全部更新, 1=仅温度, 2=仅湿度)
 */
void updateSensorData(uint8_t updateMode) {
  unsigned long currentMillis = millis();
  
  // 检查是否到了读取间隔时间
  if (currentMillis - lastSensorReadTime >= SENSOR_READ_INTERVAL) {
    // 更新传感器数据
    if (shtc3Initialized) {
      readSHTC3(true, updateMode);
    }
    
    lastSensorReadTime = currentMillis;
  }
}

/**
 * 获取当前温度值
 * 返回值: 当前温度(摄氏度)，如果传感器未初始化则返回0
 */
float getTemperature() {
  return temperature;
}

/**
 * 获取当前湿度值
 * 返回值: 当前湿度(百分比)，如果传感器未初始化则返回0
 */
float getHumidity() {
  return humidity;
}



/**
 * 获取传感器状态的JSON格式字符串
 * 用于通过蓝牙或网络传输传感器数据
 */
String getSensorDataJson() {
  char jsonData[100];
  sprintf(jsonData, "{\"temp\":%.1f,\"humi\":%.1f}", 
          temperature, 
          humidity);
  return String(jsonData);
}

/**
 * 显示温度数据到屏幕
 * 参数: t - 温度值
 */
void displayTemperature(float t) {
  // 设置局部刷新窗口 - 扩大宽度从50到80，确保显示完整温度值
  display.setPartialWindow(TEMP_X, TEMP_Y, 90, 30);

  display.firstPage();
  do {
    // 先清除区域 - 同样扩大清除区域
    display.fillRect(TEMP_X, TEMP_Y, 80, 30, GxEPD_WHITE);
    
    // 绘制温度图标
    display.drawInvertedBitmap(TEMP_X, TEMP_Y, Bitmap_tempSHT30, 16, 16, heise);
    
    // 构造温度显示字符串
    char tempStr[30];
    sprintf(tempStr, "%.1f°C", t); // 修改为保留两位小数
    
    // 显示温度数据
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312b);
    u8g2Fonts.setCursor(TEMP_X+16, TEMP_Y+15);
    u8g2Fonts.print(tempStr);
  } while (display.nextPage());
}

/**
 * 显示湿度数据到屏幕
 * 参数: h - 湿度值
 */
void displayHumidity(float h) {
  // 设置局部刷新窗口 - 扩大宽度从50到80，确保显示完整湿度值
  display.setPartialWindow(TEMP_X, TEMP_Y+20, 80, 30);

  display.firstPage();
  do {
    // 先清除区域 - 同样扩大清除区域
    display.fillRect(TEMP_X, TEMP_Y+20, 80, 30, GxEPD_WHITE);
    
    // 绘制湿度图标
    display.drawInvertedBitmap(TEMP_X, TEMP_Y+20, Bitmap_humiditySHT30, 16, 16, heise);
    
    // 构造湿度显示字符串
    char humiStr[30];
    sprintf(humiStr, "%.2f %%", h); // 修改为保留两位小数
    
    // 显示湿度数据
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312b);
    u8g2Fonts.setCursor(TEMP_X+16, TEMP_Y+35);
    u8g2Fonts.print(humiStr);
  } while (display.nextPage());
}

/**
 * 显示传感器数据到屏幕(同时显示温度和湿度)
 * 参数: t - 温度值, h - 湿度值
 */
void displaySensorData(float t, float h) {
  displayTemperature(t);
  //displayHumidity(h);
}
