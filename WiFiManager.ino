/**
 * WiFi连接管理模块
 * 负责处理WiFi连接、断开和状态监控
 * 使用WiFiManager库实现自动配网
 */

#include <WiFiManager.h> // 引入WiFiManager库

// 创建一个WiFiManager实例
WiFiManager wifiManager;

// 外部引用display对象
extern GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display;

// 外部函数引用
extern void drawCenteredText(const char* text, int16_t x, int16_t y, bool isInverted);

#include "config.h"  // 使用config.h中定义的WiFi相关常量

// WiFiManager配置参数
bool portalRunning = false;              // 标记配置门户是否正在运行

// 配置参数保存回调函数
void saveConfigCallback() {
  // WiFi参数已保存的回调函数
  // 可以在这里添加其他操作，如保存参数到EEPROM等
}

// WiFiManager配置模式回调函数
void configModeCallback(WiFiManager *myWiFiManager) {
  // 在屏幕上显示WiFi配置信息
  display.setFullWindow();
  display.firstPage();
  do {
    drawCenteredText("WiFi配置", display.width() / 2, 100, false);
    drawCenteredText("首次使用请连接热点:", display.width() / 2, 130, false);
    drawCenteredText(AP_NAME, display.width() / 2, 150, false);
    drawCenteredText((String("密码: ") + AP_PASSWORD).c_str(), display.width() / 2, 170, false);
    drawCenteredText("然后打开192.168.4.1配置", display.width() / 2, 190, false);
  } while (display.nextPage());
}

// 连接WiFi函数，使用WiFiManager库实现自动配网
bool connectWiFi() {
  // 设置回调函数
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setAPCallback(configModeCallback);
  
  // 设置配置热点
  wifiManager.setAPStaticIPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  
  // 设置WiFi连接超时时间(秒)
  wifiManager.setConnectTimeout(20); // 尝试连接WiFi的超时时间为20秒
  
  // 设置配置门户超时
  wifiManager.setConfigPortalTimeout(AP_TIMEOUT);
  
  // 尝试连接WiFi，如果失败则启动配置门户
  if (wifiManager.autoConnect(AP_NAME, AP_PASSWORD)) {
    portalRunning = false;
    return true;
  } else {
    portalRunning = false;
    return false;
  }
}

// 断开WiFi连接，节省电力
void disconnectWiFi() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  // 更新WiFi状态显示为断开状态
  displayWiFiStatus();
  // Serial.println("WiFi已断开连接");
}

// 检查WiFi连接状态，如果断开则尝试重连
bool checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    // Serial.println("WiFi连接已断开，尝试重新连接...");
    return connectWiFi();
  }
  return true;
}

// 重置WiFi配置，清除保存的凭据
bool resetWiFiSettings() {
  // Serial.println("正在重置WiFi设置...");
  wifiManager.resetSettings();
  // Serial.println("WiFi设置已重置");
  return true;
}

// 启动配置门户，不阻塞程序执行
bool startConfigPortal() {
  if (!portalRunning) {
    // Serial.println("启动配置门户...");
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setConfigPortalTimeout(AP_TIMEOUT);
    
    // 创建配置热点，不阻塞
    if (wifiManager.startConfigPortal(AP_NAME, AP_PASSWORD)) {
      // Serial.println("配置门户已启动");
      portalRunning = true;
      return true;
    } else {
      // Serial.println("无法启动配置门户");
      portalRunning = false;
      return false;
    }
  }
  return false;
}

/**
 * 通过指定的SSID和密码连接WiFi
 * @param ssid WiFi的SSID
 * @param password WiFi的密码
 * @return 连接是否成功
 */
bool connectToWiFi(const String& ssid, const String& password) {
  // Serial.printf("尝试连接WiFi: SSID=%s, 密码=%s\n", ssid.c_str(), password.c_str());
  
  // 断开当前可能存在的WiFi连接
  WiFi.disconnect(true);
  
  // 设置WiFi模式为STA模式
  WiFi.mode(WIFI_STA);
  
  // 开始连接WiFi
  WiFi.begin(ssid.c_str(), password.c_str());
  
  // 等待连接，最多等待20秒
  int timeout = 20; // 20秒超时
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(1000);
    // Serial.print(".");
    timeout--;
  }
  
  // Serial.println("");
  
  // 检查连接结果
  if (WiFi.status() == WL_CONNECTED) {
    // Serial.println("WiFi连接成功!");
    // Serial.print("IP地址: ");
    // Serial.println(WiFi.localIP());
    
    // 更新WiFi状态显示
    displayWiFiStatus();
    
    return true;
  } else {
    // Serial.println("WiFi连接失败!");
    // 连接失败后关闭WiFi以节省电力
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    
    // 更新WiFi状态显示为断开状态
    displayWiFiStatus();
    
    return false;
  }
}
