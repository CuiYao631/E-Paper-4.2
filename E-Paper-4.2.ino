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
  // 开机画面
  DrawImage();
  // 开机进度条
  onProgressBar(1, progressBarX + 30, "加载系统引导中...");
  // 初始化

  onProgressBar(10, progressBarX + 30, "加载系统文件中...");
  // 加载文件

  onProgressBar(30, progressBarX + 30, "加载系统配置文件中...");
  // 加载系统配置

  onProgressBar(60, progressBarX + 50, "正在连接WIFI...");
  // 连接wifi

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
    onProgressBar(70, progressBarX + 60, "WIFI连接成功");
    onProgressBar(100, progressBarX + 70, "加载完成");
  } else {
    Serial.println("WiFi connection failed.");
    onProgressBar(60, progressBarX + 50, "WIFI连接失败");
  }

  // 配置和同步NTP
  //configTime(3600 * 8, 0, "pool.ntp.org");
 // 清空屏幕
  BW_refresh();
  // 初次更新时间和天气
  updateWeather();
  syncTime();
  updateTime();

  // 初始化时间戳
  previousTimeMillis = millis();
  previousWeatherMillis = millis();

 
  // 显示屏休眠
  display.hibernate();
}

void loop() {
  unsigned long currentMillis = millis();

  // 检查是否需要更新天气信息
    if (currentMillis - previousWeatherMillis >= weatherInterval) {
        updateWeather();
        previousWeatherMillis = currentMillis;
    }

  // 检查是否需要更新时间
    updateTime();

  delay(1000);
}

void updateWeather() {
  // 获取天气信息
  String weatherUrl = "http://t.weather.itboy.net/api/weather/city/101110101";
  HTTPClient http;
  http.begin(weatherUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    const String payload = http.getString();

    // 解析天气信息
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    const char *hightTemp = doc["data"]["forecast"][0]["high"];
    const char *lowTemp = doc["data"]["forecast"][0]["low"];
    const char *weather = doc["data"]["forecast"][0]["type"];
    const char *fx = doc["data"]["forecast"][0]["fx"];
    const char *fl = doc["data"]["forecast"][0]["fl"];
    const char *date = doc["date"];
    // 解析年份、月份和日期
   
    // 分割日期字符串
    strncpy(riqi.year, date, 4);
    riqi.year[4] = '\0';  // 添加字符串结束符

    strncpy(riqi.month, date + 4, 2);
    riqi.month[2] = '\0';  // 添加字符串结束符

    strncpy(riqi.day, date + 6, 2);
    riqi.day[2] = '\0';  // 添加字符串结束符
    String currentDate = "日期:" + String(riqi.year) + "-" + String(riqi.month) + "-" + String(riqi.day);
    display_partialLine(3, currentDate);

    String tq = "天气:" + String(weather);
    display_partialLine(4, tq);

    String zgwd = "最高温度:" + String(hightTemp);
    display_partialLine(13, zgwd);

    String zdwd = "最低温度:" + String(lowTemp);
    display_partialLine(14, zdwd);

    String f_x = "风向:" + String(fx);
    display_partialLine(15, f_x);

    String f_l = "风力:" + String(fl);
    display_partialLine(16, f_l);

    syncTime();
  }

  http.end();
}

void updateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("无法获取时间");
    return;
  }

  RTC_minute = timeinfo.tm_min;
  RTC_hour = timeinfo.tm_hour;
  RTC_seconds = timeinfo.tm_sec;

  //如果时间有变化就刷新时间
  if(RTC_hour!=riqi.hours|| RTC_minute!=riqi.minutes){
   
      String hour, minute, assembleTime;
      if (RTC_hour < 10)   hour = "0" + String(RTC_hour);
      else                 hour = String(RTC_hour);
      if (RTC_minute < 10) minute = "0" + String(RTC_minute);
      else                 minute = String(RTC_minute);
    assembleTime = hour + ":" + minute;
    Serial.println(assembleTime);
    u8g2Fonts.setFont(u8g2_font_logisoso92_tn);
    const char *character = assembleTime.c_str();
    
    uint16_t zf_width = u8g2Fonts.getUTF8Width(character); // 获取字符的像素宽度
    int16_t fontAscent = u8g2Fonts.getFontAscent(); // 获取字体上升高度
    int16_t fontDescent = u8g2Fonts.getFontDescent(); // 获取字体下降高度
    int16_t zf_height = fontAscent - fontDescent; // 计算字符高度
  
    uint16_t x = (display.width() / 2) - (zf_width / 2); // 计算字符居中的X坐标（屏幕宽度/2 - 字符宽度/2）
    int16_t y = ((display.height() / 2) + (zf_height / 2)/2); // 计算字符居中的Y坐标（屏幕高度/2 + 字符高度/2）
  
    // 设置局部刷新窗口，窗口的高度应该是字体的高度
    display.setPartialWindow(x, y - fontAscent, zf_width, fontAscent - fontDescent);
    display.firstPage();
    do {
      u8g2Fonts.setCursor(x, y);
      u8g2Fonts.print(assembleTime);
    } while (display.nextPage());
    
    riqi.hours=RTC_hour;
    riqi.minutes=RTC_minute;
  }
}

void setCustomTime(int year, int month, int day, int hour, int minute, int second) {
  struct tm tm;
  tm.tm_year = year - 1900; // 从1900年开始
  tm.tm_mon = month - 1;    // 从0开始
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_sec = second;
  time_t t = mktime(&tm);
  struct timeval tv = {.tv_sec = t};
  settimeofday(&tv, NULL);
}

void syncTime() {
  // 获取NTP时间
  timeClient.update();
   
  riqi.hours = timeClient.getHours();
  riqi.minutes = timeClient.getMinutes();
  riqi.seconds = timeClient.getSeconds();

  Serial.print(riqi.hours);
  Serial.print(":");
  Serial.print(riqi.minutes);
  Serial.print(":");
  Serial.println(riqi.seconds);
  

  // 时间设置到RTC中
  setCustomTime(atoi(riqi.year), atoi(riqi.month), atoi(riqi.day), riqi.hours, riqi.minutes, riqi.seconds);
}
