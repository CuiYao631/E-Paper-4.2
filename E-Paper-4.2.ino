/**
 * @author: XCuiTech
 * @description: 4.2寸墨水屏桌面通知摆件
 * @date: 2021-08-06
 * @version: 0.0.1
 */
 
#include <Adafruit_GFX.h>
#include <GxEPD2_2IC_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "WiFi.h"
#include "btn.h"
#include <U8g2_for_Adafruit_GFX.h>//中文库

// 初始化墨水屏
GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 27,/*CS1=*/26,/*DC=*/ 5, /*RST=*/ 17, /*BUSY=*/ 16)); // GDEH042A03-A1
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

// 定义进度条参数
int16_t progressBarX = display.width() / 2 / 2;
int16_t progressBarY = 200;
int16_t progressBarWidth = 200;
int16_t progressBarHeight = 20;
int16_t progress = 0;


void setup()
{
  // 初始化串口
  Serial.begin(115200);
  // 初始化墨水屏
  display.init(115200);
  u8g2Fonts.begin(display);
  u8g2Fonts.setFontDirection(0);      
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);             // 设置前景色
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);             // 设置背景色
  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
  //开机加载

  //初始化WIFI
  //wifi_connect();
  //初始化按键
  //BtnI nit();
  //开机画面
  
  //显示Hello World
  // helloWorld();
  // delay(1000);
  // helloWorld();
  // delay(1000);
  // helloWorld();
  // delay(1000);
  // 清空屏幕
  display.fillScreen(GxEPD_WHITE);
  DrawRectangle();
  DrawCircle();
  DrawFont();
  //显示屏休眠
  display.hibernate();
}

const char HelloWorld[] = "Hello World!";

void helloWorld()
{
  display.setRotation(0);
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

//绘制矩形
void DrawRectangle(){
  // 设置矩形的坐标和大小
  int16_t x = 10;
  int16_t y = 10;
  int16_t width = 50;
  int16_t height = 50;

  // 设置矩形的颜色
  uint16_t color = GxEPD_BLACK;

  // 绘制矩形
  display.drawRect(x, y, width, height, color);


  // 更新显示
  display.display();
}

//绘制圆形
void DrawCircle(){
  int16_t x = display.width() / 2; // 屏幕中心点的 x 坐标
  int16_t y = display.height() / 2; // 屏幕中心点的 y 坐标
  int16_t radius = 30; // 圆形的半径

  // 设置圆形的颜色
  uint16_t color = GxEPD_BLACK;

  // 绘制圆形
  display.drawCircle(x, y, radius, color);

  // 更新显示
  display.display(true);
   // 等待一段时间
  delay(5000);
}
//显示文字
void DrawFont(){
  u8g2Fonts.setCursor(0, 80);
  u8g2Fonts.println("不要温顺的走进那个良夜，");
  u8g2Fonts.println("激情不能被消沉的暮色淹没，");
  u8g2Fonts.println("咆哮吧，咆哮，痛斥那光的退缩。");
  u8g2Fonts.println("智者在临终的时候，对黑暗妥协，");
  u8g2Fonts.println("是因为它们的语言已黯然失色，");//");
  u8g2Fonts.println("它们不想被夜色迷惑，");
  u8g2Fonts.println("咆哮吧，咆哮，痛斥那光的退缩。");
  // 更新显示
  display.display(true);
}

void loop() {
 // 设置进度条背景颜色
  uint16_t backgroundColor = GxEPD_WHITE;
  // 绘制进度条背景
  display.drawRect(progressBarX, progressBarY, progressBarWidth, progressBarHeight, GxEPD_BLACK);

  // 设置进度条填充颜色
  uint16_t progressColor = GxEPD_BLACK;
  // 计算填充矩形的宽度
  int16_t progressWidth = map(progress, 0, 100, 0, progressBarWidth);
  // 绘制进度条填充矩形
  display.fillRect(progressBarX, progressBarY, progressWidth, progressBarHeight, progressColor);
  u8g2Fonts.setCursor(display.width() / 2, progressBarY+20);
  u8g2Fonts.print(progress);
  u8g2Fonts.println("%");

  // 更新显示
  display.display(true);

  // 增加进度
  progress += 1;
  if (progress > 100) {

    progress = 0;
  }

  // 等待一段时间
  delay(200);

};
