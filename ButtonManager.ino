/*
 * 按钮管理
 * 使用Button2库配置四个按钮
 * 引脚在config.h中定义
*/
#include "config.h"
#include <Button2.h>

// 提前声明按钮回调函数
void btn1ClickHandler(Button2& btn);
void btn1LongClickHandler(Button2& btn);
void btn2ClickHandler(Button2& btn);
void btn2LongClickHandler(Button2& btn);
void btn3ClickHandler(Button2& btn);
void btn3LongClickHandler(Button2& btn);
void btn4ClickHandler(Button2& btn);
void btn4LongClickHandler(Button2& btn);

// 创建按钮对象
Button2 btn1;
Button2 btn2;
Button2 btn3;
Button2 btn4;

// 按钮功能初始化
void setupButtons() {
  Serial.println("初始化按钮...");
  
  // 配置按钮引脚及模式
  btn1.begin(BTN_PIN1);
  btn2.begin(BTN_PIN2);
  btn3.begin(BTN_PIN3);
  btn4.begin(BTN_PIN4);
  
  // 设置防抖时间（毫秒）
  btn1.setDebounceTime(50);
  btn2.setDebounceTime(50);
  btn3.setDebounceTime(50);
  btn4.setDebounceTime(50);
  
  // 配置按钮事件回调
  btn1.setClickHandler(btn1ClickHandler);
  btn1.setLongClickHandler(btn1LongClickHandler);
  
  btn2.setClickHandler(btn2ClickHandler);
  btn2.setLongClickHandler(btn2LongClickHandler);
  
  btn3.setClickHandler(btn3ClickHandler);
  btn3.setLongClickHandler(btn3LongClickHandler);
  
  btn4.setClickHandler(btn4ClickHandler);
  btn4.setLongClickHandler(btn4LongClickHandler);
  
  Serial.println("按钮初始化完成");
}

// 按钮状态更新
void updateButtons() {
  btn1.loop();
  btn2.loop();
  btn3.loop();
  btn4.loop();
}

// 按钮1点击回调函数
void btn1ClickHandler(Button2& btn) {
  Serial.println("按钮1被点击");
  // 这里添加按钮1点击时的操作
  // 例如: 切换显示模式
  if (eepUserSet.runMode < 3) {
    eepUserSet.runMode++;
  } else {
    eepUserSet.runMode = 1;
  }
  Serial.print("切换到模式: ");
  Serial.println(eepUserSet.runMode);
}

// 按钮1长按回调函数
void btn1LongClickHandler(Button2& btn) {
  Serial.println("按钮1被长按");
  // 清除WiFi配置并重启
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setCursor(50, 100);
    u8g2Fonts.print("正在清除WiFi配置...");
    u8g2Fonts.setCursor(50, 130);
    u8g2Fonts.print("设备将在3秒后重启");
  } while (display.nextPage());
  
  // 调用函数重置WiFi设置
  resetWiFiSettings();
  Serial.println("WiFi配置已清除，准备重启...");
  
  // 等待3秒后重启设备
  delay(3000);
  ESP.restart();
}

// 按钮2点击回调函数
void btn2ClickHandler(Button2& btn) {
  Serial.println("按钮2被点击");
  // 这里添加按钮2点击时的操作
  // 例如: 刷新当前显示
  if (eepUserSet.runMode == 1) { // 假设1是天气模式
    // 刷新天气信息
    Serial.println("刷新天气信息");
  } else if (eepUserSet.runMode == 2) { // 假设2是阅读模式
    // 下一页
    Serial.println("阅读模式下一页");
  } else if (eepUserSet.runMode == 3) { // 假设3是时钟模式
    // 手动同步时间
    Serial.println("手动同步时间");
    syncTime();
  }
}

// 按钮2长按回调函数
void btn2LongClickHandler(Button2& btn) {
  Serial.println("按钮2被长按");
  // 这里添加按钮2长按时的操作
}

// 按钮3点击回调函数
void btn3ClickHandler(Button2& btn) {
  Serial.println("按钮3被点击");
  // 这里添加按钮3点击时的操作
  // 例如: 在阅读模式下上一页
  if (eepUserSet.runMode == 2) { // 阅读模式
    Serial.println("阅读模式上一页");
  }
}

// 按钮3长按回调函数
void btn3LongClickHandler(Button2& btn) {
  Serial.println("按钮3被长按");
  // 这里添加按钮3长按时的操作
  // 例如: 切换显示方向
  eepUserSet.setRotation = !eepUserSet.setRotation;
  Serial.print("切换屏幕方向: ");
  Serial.println(eepUserSet.setRotation ? "270度" : "90度");
}

// 按钮4点击回调函数
void btn4ClickHandler(Button2& btn) {
  Serial.println("按钮4被点击");
  // 这里添加按钮4点击时的操作
  // 例如: 返回主菜单
  eepUserSet.runMode = 0; // 模式选择页面
  Serial.println("返回模式选择页面");
}

// 按钮4长按回调函数
void btn4LongClickHandler(Button2& btn) {
  Serial.println("按钮4被长按");
  // 这里添加按钮4长按时的操作
  // 例如: 进入深度睡眠模式
  Serial.println("准备进入深度睡眠模式");
  // 这里可以调用睡眠函数
  // ESP.deepSleep(0); // 无限期睡眠直到外部中断
}
