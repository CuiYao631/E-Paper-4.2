/**
 * @author: XCuiTech
 * @description: 4.2寸墨水屏桌面通知摆件
 * @date: 2021-08-06
 * @version: 0.0.1
 */

#include <GxEPD2_2IC_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "WiFi.h"
#include "btn.h"

// 初始化墨水屏
GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=5*/ 33,/*CS1=*/32,/*DC=*/ 26, /*RST=*/ 25, /*BUSY=*/ 14)); // GDEH042A03-A1

void setup()
{
  // 初始化串口
  Serial.begin(115200);
  // 初始化墨水屏
  display.init(115200);
  //初始化WIFI
  wifi_connect();
  //初始化按键
  BtnInit();
  //开机画面
  
  //显示Hello World
  helloWorld();
  //显示屏休眠
  //display.hibernate();
}

const char HelloWorld[] = "Hello World!";

void helloWorld()
{
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(HelloWorld);
  }
  while (display.nextPage());
}

void loop() {};
