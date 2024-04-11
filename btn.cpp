/**
 * @author: XCuiTech
 * @description: 按钮配置
 */


#include <EasyButton.h>
#include "config.h"
#include "btn.h"

EasyButton button1(PIN_BTN_1);         // 开关初始化1
EasyButton button2(PIN_BTN_2);        // 开关初始化2
EasyButton button3(PIN_BTN_3);        // 开关初始化3
EasyButton button4(PIN_BTN_4);        // 开关初始化4

void BtnInit(){
  button1.onPressed(onPressed);                         // 定义按键单按事件回调
  button1.onPressedFor(DURATION, onPressedForDuration); // 定义按键长按事件回调
}
// 单击事件函数
void onPressed()
{
  Serial.println("Button pressed!");
}
// 长按事件函数
void onPressedForDuration()
{
  Serial.println("Button pressed for 3 seconds!");
}
// 按键轮询
void BtnLoop(){
  button1.read();
}