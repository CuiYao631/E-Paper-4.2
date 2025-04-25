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

// 蓝牙管理功能声明
extern void initBluetooth();
extern void handleBluetooth();
extern void forceUpdateAdvertising();

// 函数前向声明
void drawCenteredText(const char *text, int16_t x, int16_t y, bool isInverted);

// 函数声明
void updateBatteryStatus(bool forceUpdate);
void readBatteryLevel();
void initBattery();

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTPAdress, TIME_ZONE_OFFSET, TIME_INTERVAL); // udp，服务器地址，时间偏移量，更新间隔
// NTPClient timeClient(ntpUDP, "pool.ntp.org");
//  初始化墨水屏
GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/PIN_CS, /*CS1=*/PIN_CS1, /*DC=*/PIN_DC, /*RST=*/PIN_RST, /*BUSY=*/PIN_BUSY)); // GDEH042A03-A1
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

// 定义进度条参数
int16_t progressBarY = 200;
int16_t progressBarWidth = 200;
int16_t progressBarHeight = 20;
int16_t progress = 0;

// WiFi配置 - 已由WiFiManager自动管理，不再需要手动配置
// 定义WiFiManager相关变量 - 实际配置在WiFiManager.ino中
extern WiFiManager wifiManager;
extern const char *AP_NAME;
extern const char *AP_PASSWORD;
extern bool portalRunning;

bool needSync = false;                   // 标记是否需要联网同步
bool wifiConnected = false;              // 跟踪WiFi连接状态

void setup()
{
  // 初始化串口
  Serial.begin(115200);
  Serial.println("系统启动中...");

  // 初始化电池检测 - 在WiFi连接前初始化电池

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
  // DrawImage();
  //  开机进度条
  onProgressBar(5, 0, "系统启动中...");

  initBattery();
  Serial.println("电池监测初始化完成");
  onProgressBar(15, 0, "电池监测初始化完成");

  if (initSDCard())
  {
    Serial.println("SD卡初始化成功!");
    onProgressBar(25, 0, "SD卡初始化成功");
  }
  else
  {
    Serial.println("SD卡初始化失败，将使用离线模式");
    onProgressBar(25, 0, "SD卡初始化失败");
  }

  // 初始化温湿度传感器
  // if (initDHT30()) {
  //   Serial.println("DHT30传感器初始化成功");
  //   onProgressBar(35, progressBarX + 30, "DHT30传感器初始化成功");
  // } else {
  //   Serial.println("DHT30传感器初始化失败");
  //   onProgressBar(35, progressBarX + 30, "DHT30传感器初始化失败");
  // }
  onProgressBar(35, 0, "DHT30传感器初始化成功");

  // 初始化光线传感器
  // if (initBH1750()) {
  //   Serial.println("BH1750传感器初始化成功");
  //   onProgressBar(45, progressBarX + 30, "BH1750传感器初始化成功");
  // } else {
  //   Serial.println("BH1750传感器初始化失败");
  //   onProgressBar(45, progressBarX + 30, "BH1750传感器初始化失败");
  // }
  onProgressBar(45, 0, "BH1750传感器初始化成功");

  // 初始化按钮
  setupButtons();
  Serial.println("按钮初始化完成");
  onProgressBar(55, 0, "按钮初始化完成");
  
  // 初始化蓝牙
  initBluetooth();
  Serial.println("蓝牙初始化完成");
  onProgressBar(65, 0, "蓝牙初始化完成");
  
  onProgressBar(75, 0, "正在配置wifi...");

  // 连接WiFi (使用WiFiManager模块)
  if (connectWiFi())
  {
    // 继续初始化流程
    Serial.println("wifi连接成功");
    onProgressBar(85, 0, "wifi连接成功");
  }
  else
  {
    // WiFi连接失败处理
    Serial.println("WiFi连接失败，显示配置信息");
    onProgressBar(85, 0, "wifi连接失败");
    // WiFi配置信息将在configModeCallback函数中显示
  }


  onProgressBar(100, 0, "欢迎使用");

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

void loop()
{
  unsigned long currentMillis = millis();
  static unsigned long lastDisplayUpdateMillis = 0; // 上次更新显示的时间
  static unsigned long lastBatteryUpdateMillis = 0; // 上次更新电池状态的时间
  static unsigned long lastHibernateMillis = 0;     // 上次休眠屏幕的时间
  static unsigned long lastLoopMillis = 0;          // 上次循环执行的时间
  static bool syncInProgress = false;               // 同步操作是否正在进行中
  
  // 循环执行频率控制，每50ms执行一次主逻辑，避免过度占用CPU
  if (currentMillis - lastLoopMillis < 50)
  {
    // 只更新按钮状态以确保按钮响应的及时性
    updateButtons();
    return;
  }
  lastLoopMillis = currentMillis;

  // 更新按钮状态
  updateButtons();
  
  // 处理蓝牙功能
  handleBluetooth();

  // 检查是否需要联网同步时间和天气(每半小时)
  if ((currentMillis - previousSyncMillis >= syncInterval || needSync) && !syncInProgress)
  {
    syncInProgress = true;  // 标记同步操作开始
    Serial.println("开始执行每半小时联网同步...");

    // 提前确认是否需要连接WiFi
    bool needWiFi = true;
    
    // 只在需要时连接WiFi，避免不必要的连接
    if (needWiFi && !wifiConnected)
    {
      if (connectWiFi())
      {
        wifiConnected = true;
        Serial.println("WiFi已连接以准备同步");
        

      // 清空屏幕并更新内容
      BW_refresh();

        // 初次更新时间和天气
        updateWeather();
        Serial.println("天气数据已更新");
        syncTime();
        Serial.println("NTP时间已同步");
        updateTime();

        // 更新时间戳并清除同步标记
        previousSyncMillis = currentMillis;
        needSync = false;
        
        // 同步完成后断开WiFi以节省电量
        disconnectWiFi();
        wifiConnected = false;
        Serial.println("同步完成，已断开WiFi连接");
      }
      else
      {
        // WiFi连接失败处理，使用指数退避策略减少重试频率
        Serial.println("WiFi连接失败，稍后重试同步");
        needSync = true;
        // 不使用阻塞延时，而是标记下次重试的时间
        static int retryCount = 0;
        static unsigned long nextRetryTime = 0;
        
        if (retryCount < 3) {
          // 前几次快速重试
          nextRetryTime = currentMillis + 30000; // 30秒后重试
          retryCount++;
        } else {
          // 之后延长重试间隔
          nextRetryTime = currentMillis + 300000; // 5分钟后重试
        }
        
        // 如果下次成功连接，重置重试计数
        if (currentMillis >= nextRetryTime) {
          syncInProgress = false;
        }
      }
    }
    
    syncInProgress = false;  // 标记同步操作结束
  }

  // 检查是否需要更新时间(每秒)
  if (currentMillis - previousTimeMillis >= timeInterval)
  {
    updateTime();
    previousTimeMillis = currentMillis;
    
    // 更新显示，减少不必要的刷新
    if (currentMillis - lastDisplayUpdateMillis >= 10000) { // 每10秒刷新一次完整显示
      display_main();
      lastDisplayUpdateMillis = currentMillis;
    }

    // 每分钟更新一次电池状态，避免频繁读取
    if (currentMillis - lastBatteryUpdateMillis >= 60000)
    { 
      // 更新电池状态前确保WiFi是断开的，以获取准确的电池读数
      bool wifiWasConnected = (WiFi.status() == WL_CONNECTED);
      if (wifiWasConnected && !syncInProgress)
      {
        WiFi.disconnect(false); // false表示断开但保留配置
        delay(50);              // 缩短延时，减少阻塞时间
      }

      // 更新电池状态（强制更新）
      updateBatteryStatus(true);
      Serial.println("已更新电池状态（每分钟）");

      // 如果之前WiFi是连接的并且需要重新连接
      if (wifiWasConnected && needSync && !syncInProgress)
      {
        WiFi.reconnect();
      }

      lastBatteryUpdateMillis = currentMillis;
    }
  }

  // 只在不需要频繁更新时休眠显示屏，以节省电力
  if (currentMillis - lastHibernateMillis >= 60000)  // 增加到60秒休眠一次
  { 
    // 检查是否有任何即将发生的操作需要活跃显示屏
    bool activityExpected = (currentMillis - previousSyncMillis >= syncInterval - 15000) || needSync;
    
    if (!activityExpected) {
      display.hibernate();
      lastHibernateMillis = currentMillis;
    }
  }
}
