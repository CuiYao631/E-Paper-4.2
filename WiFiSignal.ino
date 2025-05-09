/**
 * WiFi信号强度检测和显示模块
 * 提供WiFi信号强度检测和在屏幕上显示的功能
 */

// 使用外部WiFi相关定义
extern WiFiClass WiFi;
#include "Arduino.h"

#define rssi_X 340 // WiFi信号图标X坐标
#define rssi_Y 2   // WiFi信号图标Y坐标

// 使用外部对象和变量，这些都已经在主文件中定义
extern GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display;
extern const unsigned char Bitmap_wifi_slash[] PROGMEM;
extern const unsigned char Bitmap_wifi_1[] PROGMEM;
extern const unsigned char Bitmap_wifi_2[] PROGMEM;
extern const unsigned char Bitmap_wifi_3[] PROGMEM;

// 定义黑色常量，与其他文件保持一致
#define heise GxEPD_BLACK

/**
 * 获取WiFi信号强度并映射为0-3的强度等级
 * 返回值: -1=未连接, 0=弱信号, 1=中低信号, 2=中高信号, 3=满格信号
 */
int getWiFiSignalStrength() {
  // 检查WiFi连接状态
  if (WiFi.status() != WL_CONNECTED) {
    return -1; // 未连接，返回-1
  }
  
  // 获取RSSI值(接收信号强度指示),通常为负值,范围约为-100到0
  int rssi = WiFi.RSSI(); 
  
  // 使用常量定义信号强度阈值，提高代码可读性
  const int EXCELLENT_SIGNAL = -55;  // -55dBm及以上为极好信号
  const int GOOD_SIGNAL = -65;       // -65dBm及以上为良好信号
  const int FAIR_SIGNAL = -75;       // -75dBm及以上为一般信号
  
  // 将RSSI映射到0-3的强度等级，使用更简洁的表达方式
  if (rssi >= EXCELLENT_SIGNAL) return 3;      // 满格信号
  else if (rssi >= GOOD_SIGNAL) return 2;      // 中高信号
  else if (rssi >= FAIR_SIGNAL) return 1;      // 中低信号
  else return 0;                              // 弱信号
}

/**
 * 根据WiFi状态显示相应的WiFi图标
 * 使用信号强度选择合适的图标并显示
 */
void drawWiFiStatus() {
    
  // 获取WiFi信号强度
  int signalStrength = getWiFiSignalStrength();
  
  // 根据信号强度选择合适的图标
  const unsigned char* wifiIcon;
  
  // 根据信号强度选择相应的图标
  switch (signalStrength) {
    case -1: // 未连接
      wifiIcon = Bitmap_wifi_slash;
      break;
    case 0:  // 弱信号
    case 1:  // 中低信号
      wifiIcon = Bitmap_wifi_1;
      break;
    case 2:  // 中高信号
      wifiIcon = Bitmap_wifi_2;
      break;
    case 3:  // 满格信号
      wifiIcon = Bitmap_wifi_3;
      break;
    default: // 默认使用未连接图标
      wifiIcon = Bitmap_wifi_slash;
      break;
  }
  
  // 显示选择的WiFi图标
  display.drawInvertedBitmap(rssi_X, rssi_Y, wifiIcon, 25, 20, heise);
  
  //调试信息可以根据需要取消注释
  if (signalStrength >= 0) {
    Serial.print("WiFi信号强度: ");
    Serial.print(signalStrength);
    Serial.println("格");
  } else {
    Serial.println("WiFi未连接");
  }
}

/**
 * 绘制WiFi状态图标，使用局部刷新方式更新屏幕
 * 这个函数会完整执行刷新周期，适合单独更新WiFi状态时使用
 */
void displayWiFiStatus()
{
  // 设置局部刷新窗口，只刷新WiFi图标区域，提高刷新速度
  display.setPartialWindow(rssi_X, rssi_Y, 25, 20); // 设置局部刷新窗口
  display.firstPage();
  do
  {
    // 绘制WiFi状态图标
    drawWiFiStatus();
  } while (display.nextPage());
}
