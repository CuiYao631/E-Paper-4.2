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
#include "image.h"
#include "config.h"
#include "Entity.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 8 * 3600, 60000); // udp，服务器地址，时间偏移量，更新间隔
// 初始化墨水屏
GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 27,/*CS1=*/26,/*DC=*/ 5, /*RST=*/ 17, /*BUSY=*/ 16)); // GDEH042A03-A1
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

// 定义进度条参数
int16_t progressBarX = display.width() / 2 / 2;
int16_t progressBarY = 200;
int16_t progressBarWidth = 200;
int16_t progressBarHeight = 20;
int16_t progress = 0;

const char* ssd = "崔Phone";
const char* pasd = "99999999";

unsigned long previousWeatherMillis = 0; // 上次更新天气的时间
unsigned long previousTimeMillis = 0; // 上次更新时间的时间
const long weatherInterval = 3600000; // 1小时的毫秒数
const long timeInterval = 60000; // 1分钟的毫秒数

void setup() {
  // 初始化串口
  Serial.begin(115200);

  // 初始化墨水屏
  display.init(115200);
  u8g2Fonts.begin(display); // 将u8g2过程连接到Adafruit GFX
  u8g2Fonts.setFontMode(1); // 使用u8g2透明模式（这是默认设置）
  u8g2Fonts.setFontDirection(0); // 从左到右（这是默认设置）
  u8g2Fonts.setForegroundColor(heise); // 设置前景色
  u8g2Fonts.setBackgroundColor(baise); // 设置背景色
  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);

  // 清空屏幕
  display.fillScreen(GxEPD_WHITE);
//  // 开机画面
//  DrawImage();
//  // 开机进度条
//  onProgressBar(1, progressBarX + 30, "加载系统引导中...");
//  // 初始化
//
//  onProgressBar(10, progressBarX + 30, "加载系统文件中...");
//  // 加载文件
//
//  onProgressBar(30, progressBarX + 30, "加载系统配置文件中...");
//  // 加载系统配置
//
//  onProgressBar(60, progressBarX + 50, "正在连接WIFI...");
//  // 连接wifi
//
  WiFi.begin(ssd, pasd);
  int retryCount = 0;
  const int maxRetries = 20;

  while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("connected success!");
    Serial.print("IP:\n");
    Serial.println(WiFi.localIP());
    //onProgressBar(70, progressBarX + 60, "WIFI连接成功");
    //onProgressBar(100, progressBarX + 70, "加载完成");
  } else {
    Serial.println("WiFi connection failed.");
    //onProgressBar(60, progressBarX + 50, "WIFI连接失败");
  }
//
//  // 配置和同步NTP
//  //configTime(3600 * 8, 0, "pool.ntp.org");
   // 清空屏幕
    BW_refresh();
   // 初次更新时间和天气
    updateWeather();
    syncTime();
    updateTime();
//
//  // 初始化时间戳
  previousTimeMillis = millis();
  previousWeatherMillis = millis();
//
// 
//  // 显示屏休眠
  display.hibernate();

  // drawTitle();
  // drawMiddle();
  // drawBottom();
}

void loop() {
 
  display_main();
  
  delay(1000);
}

