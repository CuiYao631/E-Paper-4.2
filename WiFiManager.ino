/**
 * WiFi连接管理模块
 * 负责处理WiFi连接、断开和状态监控
 */

// 连接WiFi函数，优化连接流程和错误处理
bool connectWiFi() {
  Serial.println("正在连接WiFi...");
  
  // 设置为站点模式，降低功耗
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssd, pasd);
  
  int retryCount = 0;
  
  while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
    delay(wifiRetryDelay);
    Serial.print(".");
    retryCount++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi连接成功!");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("\nWiFi连接失败，请检查网络或凭据");
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
