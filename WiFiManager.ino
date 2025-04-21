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

// WiFiManager配置参数
const char* AP_NAME = "E-Paper-Config";  // 热点名称
const char* AP_PASSWORD = "12345678";    // 热点密码，为空则无密码
const int AP_TIMEOUT = 180;              // 配置门户超时时间（秒）
bool portalRunning = false;              // 标记配置门户是否正在运行

// 配置参数保存回调函数
void saveConfigCallback() {
  Serial.println("配置已保存");
  // TODO: 如需要，可以在这里添加其他操作，如保存参数到EEPROM等
}

// WiFiManager配置函数
void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("已进入配置模式");
  Serial.print("请连接到热点: ");
  Serial.println(AP_NAME);
  Serial.print("配置页面IP: ");
  Serial.println(WiFi.softAPIP());
  
  // 使用WiFiManager连接WiFi
  display.setFullWindow();
  // 在屏幕上显示WiFi配置提示
  display.firstPage();
  do {
    drawCenteredText("WiFi配置", display.width() / 2, 100, false);
    drawCenteredText("首次使用请连接热点:", display.width() / 2, 130, false);
    drawCenteredText(AP_NAME, display.width() / 2, 150, false);
    drawCenteredText("密码: 12345678", display.width() / 2, 170, false);
    drawCenteredText("然后打开192.168.4.1配置", display.width() / 2, 190, false);
  } while (display.nextPage());
}

// 连接WiFi函数，使用WiFiManager库实现自动配网
bool connectWiFi() {
  Serial.println("正在连接WiFi...");
  
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
    Serial.println("\nWiFi连接成功!");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
    portalRunning = false;
    return true;
  } else {
    Serial.println("配置门户已关闭，WiFi未连接");
    portalRunning = false;
    return false;
  }
}

// 断开WiFi连接，节省电力
void disconnectWiFi() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi已断开连接");
}

// 检查WiFi连接状态，如果断开则尝试重连
bool checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi连接已断开，尝试重新连接...");
    return connectWiFi();
  }
  return true;
}

// 重置WiFi配置，清除保存的凭据
bool resetWiFiSettings() {
  Serial.println("正在重置WiFi设置...");
  wifiManager.resetSettings();
  Serial.println("WiFi设置已重置");
  return true;
}

// 启动配置门户，不阻塞程序执行
bool startConfigPortal() {
  if (!portalRunning) {
    Serial.println("启动配置门户...");
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setConfigPortalTimeout(AP_TIMEOUT);
    
    // 创建配置热点，不阻塞
    if (wifiManager.startConfigPortal(AP_NAME, AP_PASSWORD)) {
      Serial.println("配置门户已启动");
      portalRunning = true;
      return true;
    } else {
      Serial.println("无法启动配置门户");
      portalRunning = false;
      return false;
    }
  }
  return false;
}
