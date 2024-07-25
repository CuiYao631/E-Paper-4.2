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
//  display.hibernate();
  String currentDate = "日期:2024年7月24日";
  drawTitle(currentDate);
  drawMiddle();
  drawBottom();
}

void loop() {
 
  display_main();
  
  delay(1000);
}
void display_tbcs() //图标测试
{
  
    
    String currentDate = "日期:2024年7月24日";
    drawTitle(currentDate);
    //绘制电量
    display.drawInvertedBitmap(370, 5, Bitmap_bat3, 21, 12, heise);
    display.drawLine(0, 35, 400, 35, 0); //顶部水平线
//    display.drawInvertedBitmap(0, 0, Bitmap_qt, 45, 45, heise);
//    display.drawInvertedBitmap(50, 0, Bitmap_dy, 45, 45, heise);
//    display.drawInvertedBitmap(100, 0, Bitmap_yt, 45, 45, heise);
//    display.drawInvertedBitmap(150, 0, Bitmap_zheny, 45, 45, heise);
//    display.drawInvertedBitmap(200, 0, Bitmap_lzybbb, 45, 45, heise);
//    display.drawInvertedBitmap(250, 0, Bitmap_xy, 45, 45, heise);
//    display.drawInvertedBitmap(300, 0, Bitmap_zhongy, 45, 45, heise);
//    display.drawInvertedBitmap(350, 0, Bitmap_dayu, 45, 45, heise);   
//    
//    display.drawInvertedBitmap(0, 40, Bitmap_by, 45, 45, heise);
//    display.drawInvertedBitmap(50, 40, Bitmap_dby, 45, 45, heise);
//    display.drawInvertedBitmap(100, 40, Bitmap_tdby, 45, 45, heise);
//    display.drawInvertedBitmap(150, 40, Bitmap_dongy, 45, 45, heise); 
//    display.drawInvertedBitmap(200, 40, Bitmap_yjx, 45, 45, heise);
//    display.drawInvertedBitmap(250, 40, Bitmap_zhenx, 45, 45, heise);
//    display.drawInvertedBitmap(300, 40, Bitmap_xx, 45, 45, heise);  
//    display.drawInvertedBitmap(350, 40, Bitmap_zhongx, 45, 45, heise); 
//    
//    display.drawInvertedBitmap(0, 80, Bitmap_dx, 45, 45, heise);
//    display.drawInvertedBitmap(100, 80, Bitmap_bx, 45, 45, heise);
//    display.drawInvertedBitmap(50, 80, Bitmap_lzy, 45, 45, heise);
//    display.drawInvertedBitmap(150, 80, Bitmap_fc, 45, 45, heise);
//    display.drawInvertedBitmap(200, 80, Bitmap_ys, 45, 45, heise);
//    display.drawInvertedBitmap(250, 80, Bitmap_scb, 45, 45, heise);
//    display.drawInvertedBitmap(300, 80, Bitmap_w, 45, 45, heise);
//    display.drawInvertedBitmap(350, 80, Bitmap_m, 45, 45, heise);
//    
//    display.drawInvertedBitmap(0, 120, Bitmap_f, 45, 45, heise);
//    display.drawInvertedBitmap(50, 120, Bitmap_jf, 45, 45, heise);
//    display.drawInvertedBitmap(100, 120, Bitmap_rdfb, 45, 45, heise);
//    display.drawInvertedBitmap(150, 120, Bitmap_ljf, 45, 45, heise);
//    display.drawInvertedBitmap(200, 120, Bitmap_wz, 45, 45, heise);
//    display.drawInvertedBitmap(250, 120, Bitmap_qt_ws, 45, 45, heise);
//    display.drawInvertedBitmap(300, 120, Bitmap_yt_ws, 45, 45, heise);
//    display.drawInvertedBitmap(350, 120, Bitmap_dy_ws, 45, 45, heise);
//    
//    display.drawInvertedBitmap(0, 160, Bitmap_zy_ws, 45, 45, heise);
//    display.drawInvertedBitmap(50, 160, Bitmap_zx_ws, 45, 45, heise);
//    display.drawInvertedBitmap(100, 180, Bitmap_weizhi, 13, 13, heise);
//    display.drawInvertedBitmap(120, 180, Bitmap_zhuangtai, 13, 13, heise);
//    display.drawInvertedBitmap(140, 180, Bitmap_gengxing, 13, 13, heise);
//    display.drawInvertedBitmap(160, 160, Bitmap_riqi, 50, 50, heise);
//    display.drawInvertedBitmap(220, 160, Bitmap_batlow, 24, 35, heise);
//    display.drawInvertedBitmap(250, 180, Bitmap_humidity, 13, 13, heise);
//    display.drawInvertedBitmap(270, 180, Bitmap_fx, 13, 13, heise);
//    display.drawInvertedBitmap(290, 180, Bitmap_tempSHT30, 16, 16, heise);
//    display.drawInvertedBitmap(310, 180, Bitmap_humiditySHT30, 16, 16, heise);
//    display.drawInvertedBitmap(330, 160, Bitmap_peiwangMod, 45, 45, heise);
//
//    display.drawInvertedBitmap(0, 200, Bitmap_shizhongMod, 45, 45, heise);
//    display.drawInvertedBitmap(50, 200, Bitmap_yueduMod, 45, 45, heise);
//    display.drawInvertedBitmap(100, 200, Bitmap_tianqiMod, 45, 45, heise);
//    display.drawInvertedBitmap(150, 220, Bitmap_zdy, 16, 16, heise);
//    display.drawInvertedBitmap(170, 220, Bitmap_dlsd, 19, 31, heise);
//    display.drawInvertedBitmap(190, 220, Bitmap_bat1, 21, 12, heise);
//    display.drawInvertedBitmap(220, 220, Bitmap_bat2, 21, 12, heise);

//    display.drawInvertedBitmap(280, 220, Bitmap_sjjg, 31, 22, heise);
//    display.drawInvertedBitmap(310, 200, Bitmap_fashe, 48, 41, heise);
//    display.drawInvertedBitmap(360, 220, Bitmap_dlxtb, 11, 16, heise);
//    display.drawInvertedBitmap(370, 220, Bitmap_wifidk, 16, 13, heise);

}
