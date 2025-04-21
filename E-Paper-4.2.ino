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
int16_t progressBarX = display.width() / 2 / 2;
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
  onProgressBar(1, progressBarX + 35, "系统启动中...");

  initBattery();
  Serial.println("电池监测初始化完成");
  onProgressBar(10, progressBarX + 30, "电池监测初始化完成");

  if (initSDCard())
  {
    Serial.println("SD卡初始化成功!");
    onProgressBar(20, progressBarX + 30, "SD卡初始化成功");
  }
  else
  {
    Serial.println("SD卡初始化失败，将使用离线模式");
    onProgressBar(20, progressBarX + 30, "SD卡初始化失败");
  }

  // 初始化温湿度传感器
  // if (initDHT30()) {
  //   Serial.println("DHT30传感器初始化成功");
  //   onProgressBar(30, progressBarX + 30, "DHT30传感器初始化成功");
  // } else {
  //   Serial.println("DHT30传感器初始化失败");
  //   onProgressBar(30, progressBarX + 30, "DHT30传感器初始化失败");
  // }
  onProgressBar(30, progressBarX + 20, "DHT30传感器初始化成功");

  // 初始化光线传感器
  // if (initBH1750()) {
  //   Serial.println("BH1750传感器初始化成功");
  //   onProgressBar(40, progressBarX + 30, "BH1750传感器初始化成功");
  // } else {
  //   Serial.println("BH1750传感器初始化失败");
  //   onProgressBar(40, progressBarX + 30, "BH1750传感器初始化失败");
  // }
  onProgressBar(40, progressBarX + 20, "BH1750传感器初始化成功");

  // 初始化按钮
  setupButtons();
  Serial.println("按钮初始化完成");
  onProgressBar(50, progressBarX + 30, "按钮初始化完成");

  onProgressBar(60, progressBarX + 30, "正在配置wifi...");

  // 连接WiFi (使用WiFiManager模块)
  if (connectWiFi())
  {
    // 继续初始化流程
    Serial.println("wifi连接成功");
    onProgressBar(70, progressBarX + 30, "wifi连接成功");
  }
  else
  {
    // WiFi连接失败处理
    Serial.println("WiFi连接失败，显示配置信息");
    onProgressBar(70, progressBarX + 30, "wifi连接失败");
    // WiFi配置信息将在configModeCallback函数中显示
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

void loop()
{
  unsigned long currentMillis = millis();

  // 更新按钮状态
  updateButtons();

  // 检查是否需要联网同步时间和天气(每小时)
  if (currentMillis - previousSyncMillis >= syncInterval || needSync)
  {
    Serial.println("开始执行每小时联网同步...");

    // 确保WiFi已连接
    if (checkWiFiConnection())
    {
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
    }
    else
    {
      // WiFi连接失败，5分钟后重试
      Serial.println("WiFi连接失败，5分钟后重试同步");
      needSync = true;
      delay(300000); // 5分钟延迟
    }
  }

  // 检查是否需要更新时间(每秒)
  if (currentMillis - previousTimeMillis >= timeInterval)
  {
    updateTime();

    // 每分钟更新一次电池状态
    static unsigned long lastBatteryUpdateMillis = 0;
    if (currentMillis - lastBatteryUpdateMillis >= 60000)
    { // 60000ms = 1分钟
      // 确保WiFi断开，以获取准确的电池读数
      bool wifiWasConnected = (WiFi.status() == WL_CONNECTED);
      if (wifiWasConnected)
      {
        // 临时断开WiFi连接
        WiFi.disconnect(false); // false表示断开但保留配置
        delay(100);             // 等待WiFi模块完全停止活动
      }

      // 更新电池状态（强制更新）
      updateBatteryStatus(true);
      Serial.println("已更新电池状态（每分钟）");

      // 如果之前WiFi是连接的，并且需要重新连接
      if (wifiWasConnected && needSync)
      {
        WiFi.reconnect();
      }

      lastBatteryUpdateMillis = currentMillis;
    }

    previousTimeMillis = currentMillis;
  }

  // 更新显示
  display_main();

  // 控制WiFi连接状态：只有在需要同步时或即将需要同步时保持WiFi连接
  bool needWiFi = (currentMillis - previousSyncMillis >= syncInterval - 10000) || needSync;

  if (needWiFi)
  {
    // 需要WiFi连接
    if (!wifiConnected)
    {
      // 当前无连接但需要连接时
      if (connectWiFi())
      {
        wifiConnected = true;
        Serial.println("WiFi已连接以准备同步");
      }
    }
  }
  else
  {
    // 不需要网络连接时
    if (wifiConnected)
    {
      // 当前已连接但不需要时断开
      disconnectWiFi();

      wifiConnected = false;
    }
  }

  // 不要每次循环都让显示屏休眠，这可能导致频繁的Busy Timeout
  // 只在不需要频繁更新时休眠显示屏
  static unsigned long lastHibernateMillis = 0;
  static unsigned long lastLoopMillis = 0;

  if (currentMillis - lastHibernateMillis >= 30000)
  { // 每30秒才休眠一次
    display.hibernate();
    lastHibernateMillis = currentMillis;
  }

  // 用非阻塞延时替代delay(1000)
  // 这样可以保证按钮响应的及时性，同时仍然给墨水屏足够的处理时间
  if (currentMillis - lastLoopMillis < 50)
  {
    // 此处不使用delay，让loop快速循环以检测按钮状态
    // 但每50ms才执行一次主要逻辑，避免过度占用CPU
    return;
  }
  lastLoopMillis = currentMillis;
}
