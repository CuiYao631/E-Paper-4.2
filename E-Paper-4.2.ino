#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Adafruit_GFX.h>
#include <GxEPD2_2IC_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <U8g2_for_Adafruit_GFX.h> //中文库
#include <esp_sleep.h>
#include <WiFiManager.h> // 引入WiFiManager库
#include "image.h"
#include "config.h"
#include "Entity.h"


// 时间相关函数声明
extern void updateTime();
extern void syncTime();
extern void updateWeather();

// 全局函数声明
void drawCenteredText(const char *text, int16_t x, int16_t y, bool isInverted);

// 电池相关函数
void updateBatteryStatus(bool forceUpdate);
void readBatteryLevel();
void initBattery();

// 传感器相关函数
bool initSensors(bool readDataAfterInit = true, bool updateDisplay = true, uint8_t updateMode = 0);
bool initSHTC3(bool readDataAfterInit = true, bool updateDisplay = true, uint8_t updateMode = 0);
bool readSHTC3(bool updateDisplay = true, uint8_t updateMode = 0);
void updateSensorData(uint8_t updateMode = 0);

// 主显示更新函数声明(实现在DisplayMain.ino)
extern void updateMainDisplay();

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTPAdress, TIME_ZONE_OFFSET, TIME_INTERVAL); // udp，服务器地址，时间偏移量，更新间隔
GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/PIN_CS, /*CS1=*/PIN_CS1, /*DC=*/PIN_DC, /*RST=*/PIN_RST, /*BUSY=*/PIN_BUSY)); // GDEH042A03-A1
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

// 进度条参数（配置在config.h中）
int16_t progress = 0;               // 当前进度值

// WiFi相关变量(在WiFiManager.ino中定义)
extern WiFiManager wifiManager;
extern bool portalRunning;

// 全局状态标志
bool needSync = false;              // 标记是否需要联网同步
bool wifiConnected = false;         // 跟踪WiFi连接状态


void setup()
{
  // 1. 初始化基础系统
  Serial.begin(115200);
  
  // 2. 初始化显示屏
  display.init(115200);
  u8g2Fonts.begin(display);
  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);
  u8g2Fonts.setForegroundColor(heise);
  u8g2Fonts.setBackgroundColor(baise);
  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312b);

  // 清空屏幕并显示启动进度
  display.fillScreen(GxEPD_WHITE);
  onProgressBar(5, 0, "系统启动中...");

  // 3. 初始化硬件模块
  // 3.1 电池管理初始化
  initBattery();
  onProgressBar(15, 0, "电池监测初始化完成");

  // 3.2 SD卡初始化
  if (initSDCard())
  {
    onProgressBar(25, 0, "SD卡初始化成功");
  }
  else
  {
    onProgressBar(25, 0, "SD卡初始化失败");
  }

  // 3.3 传感器初始化
  if (initSensors(true, true, 0)) {
    onProgressBar(35, 0, "传感器初始化成功");
  } else {
    onProgressBar(35, 0, "传感器初始化失败");
  }
  onProgressBar(45, 0, "传感器数据已更新");

  // 3.4 按钮初始化
  setupButtons();
  onProgressBar(55, 0, "按钮初始化完成");
  
  // 4. 网络连接
  onProgressBar(75, 0, "正在配置WIFI...");

  // 连接WiFi (使用WiFiManager模块)
  if (connectWiFi())
  {
    onProgressBar(85, 0, "WIFI连接成功");
  }
  else
  {
    onProgressBar(85, 0, "WIFI连接失败");
    // WiFi配置信息将在configModeCallback函数中显示
  }

  onProgressBar(100, 0, "欢迎使用");

  // 5. 清空屏幕并准备主显示内容
  BW_refresh();

  // 6. 初始数据更新
  updateWeather();     // 获取天气数据
  syncTime();          // 同步NTP时间
  updateTime();        // 更新显示时间
  updateSensorData(0); // 更新传感器数据
  
  // 显示WiFi状态
  displayWiFiStatus(); // 在屏幕上显示WiFi状态

  // 7. 初始化时间戳
  previousTimeMillis = millis();
  previousWeatherMillis = millis();

  // 8. 显示屏进入休眠模式以节省电力
  display.hibernate();
}

void loop()
{
  // 调用DisplayMain中的updateMainDisplay函数处理主要显示逻辑
  updateMainDisplay();
}
