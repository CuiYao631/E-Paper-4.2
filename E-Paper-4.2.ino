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
#include "image.h"
#include "config.h"
#include "Entity.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 8 * 3600, 60000); // udp，服务器地址，时间偏移量，更新间隔
//NTPClient timeClient(ntpUDP, "pool.ntp.org");
// 初始化墨水屏
GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 27,/*CS1=*/26,/*DC=*/ 5, /*RST=*/ 17, /*BUSY=*/ 16)); // GDEH042A03-A1
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

// 定义进度条参数
int16_t progressBarX = display.width() / 2 / 2;
int16_t progressBarY = 200;
int16_t progressBarWidth = 200;
int16_t progressBarHeight = 20;
int16_t progress = 0;

// WiFi配置
const char* ssd = "infiprimes";
const char* pasd = "zhongmao15ZZ";

// 定时器和更新间隔常量
unsigned long previousWeatherMillis = 0; // 上次更新天气的时间
unsigned long previousTimeMillis = 0;    // 上次更新时间的时间
unsigned long previousSyncMillis = 0;    // 上次联网同步的时间(天气和NTP)
const long syncInterval = 3600000;       // 1小时的毫秒数(联网同步天气和时间)
const long timeInterval = 1000;          // 1秒的毫秒数(更新时间)
const int maxRetries = 20;               // 最大WiFi重试次数
const int wifiRetryDelay = 500;          // WiFi重试延迟(毫秒)
const int deepSleepInterval = 60;        // 深度睡眠时间(秒)
bool needSync = false;                   // 标记是否需要联网同步
bool wifiConnected = false;              // 跟踪WiFi连接状态

void setup() {
  // 初始化串口
  Serial.begin(115200);
  Serial.println("系统启动中...");

  // 初始化电池检测
  //initBattery();
  Serial.println("电池监测初始化完成");

  if (initSDCard()) {
    Serial.println("SD卡初始化成功!");
  } else {
    Serial.println("SD卡初始化失败，将使用离线模式");
  }
  
  // 初始化墨水屏
  display.init(115200);
  u8g2Fonts.begin(display);
  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);
  u8g2Fonts.setForegroundColor(heise);
  u8g2Fonts.setBackgroundColor(baise);
  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);

  // 清空屏幕
  display.fillScreen(GxEPD_WHITE);
  
  // 连接WiFi (使用新的WiFiManager模块)
  if (connectWiFi()) {
    // 继续初始化流程
    Serial.println("正在初始化系统...");
  } else {
    // WiFi连接失败处理
    Serial.println("WiFi连接失败，显示离线内容");
    // 可以在这里添加显示离线信息的代码
  }
  
  // 清空屏幕并更新内容
  BW_refresh();
  
  // 初次更新时间和天气
  updateWeather();
  syncTime();
  updateTime();
  
  // 初始化时间戳
  previousTimeMillis = millis();
  previousWeatherMillis = millis();
  
  // 显示屏休眠，节省电力
  display.hibernate();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // 更新电池状态
  //updateBatteryStatus();
  
  // 检查是否需要联网同步时间和天气(每小时)
  if (currentMillis - previousSyncMillis >= syncInterval || needSync) {
    Serial.println("开始执行每小时联网同步...");
    
    // 确保WiFi已连接
    if (checkWiFiConnection()) {
      // 同步时间
      syncTime();
      Serial.println("NTP时间已同步");
      
      // 更新天气
      updateWeather();
      Serial.println("天气数据已更新");
      
      // 更新时间戳并清除同步标记
      previousSyncMillis = currentMillis;
      needSync = false;
      
      // 全屏刷新一次，确保所有数据都正确显示
      BW_refresh();
    } else {
      // WiFi连接失败，5分钟后重试
      Serial.println("WiFi连接失败，5分钟后重试同步");
      needSync = true;
      delay(300000); // 5分钟延迟
    }
  }
  
  // 检查是否需要更新时间(每秒)
  if (currentMillis - previousTimeMillis >= timeInterval) {
    updateTime();
    previousTimeMillis = currentMillis;
  }
  
  // 更新显示
  display_main();
  
  // 控制WiFi连接状态：只有在需要同步时或即将需要同步时保持WiFi连接
  bool needWiFi = (currentMillis - previousSyncMillis >= syncInterval - 10000) || needSync;
  
  if (needWiFi) {
    // 需要WiFi连接
    if (!wifiConnected) {
      // 当前无连接但需要连接时
      if (connectWiFi()) {
        wifiConnected = true;
        Serial.println("WiFi已连接以准备同步");
      }
    }
  } else {
    // 不需要网络连接时
    if (wifiConnected) {
      // 当前已连接但不需要时断开
      disconnectWiFi();
      wifiConnected = false;
    }
  }
  
  // 不要每次循环都让显示屏休眠，这可能导致频繁的Busy Timeout
  // 只在不需要频繁更新时休眠显示屏
  static unsigned long lastHibernateMillis = 0;
  if (currentMillis - lastHibernateMillis >= 30000) { // 每30秒才休眠一次
    display.hibernate();
    lastHibernateMillis = currentMillis;
  }
  
  // 延长延迟时间，给墨水屏更多时间处理命令
  delay(1000);
}

