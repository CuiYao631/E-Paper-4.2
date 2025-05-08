/**
 * @file DisplayMain.ino
 * @brief 主显示管理，处理天气数据和屏幕布局
 * @author xiaocui
 * @date 2025-04-21
 * 
 * 此文件负责处理天气数据的获取、解析和显示，
 * 以及管理屏幕的主要布局和元素位置定义
 */

// 外部变量和函数声明
extern bool needSync;                    // 是否需要同步
extern bool wifiConnected;               // WiFi连接状态
extern unsigned long previousTimeMillis; // 上次更新时间的时间戳
extern unsigned long previousSyncMillis; // 上次同步的时间戳
// 使用正确的类型与config.h保持一致
extern const long timeInterval;          // 时间更新间隔
extern const long syncInterval;          // 同步更新间隔
extern void updateTime();                // 更新时间函数
extern void updateSensorData(uint8_t);   // 更新传感器数据函数
extern void updateBatteryStatus(bool);   // 更新电池状态函数
extern class GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display; // 显示屏对象

// ===== 布局常量定义 =====
// 使用config.h中定义的常量

// 未来天气详情相关位置
uint16_t day_x0 = 54;        // 未来天气详情X坐标 (在代码中动态计算)

// 实况温度位置
uint16_t temp_x;             // 温度X位置(动态计算)
uint16_t circle_x;           // 圈圈位置(动态计算)


/**
 * @brief 获取并更新日期和天气信息
 * 
 * 从API获取最新天气数据，解析JSON格式
 * 并更新屏幕上的天气信息显示
 */
void updateWeather() {
  // 获取天气信息
  HTTPClient http;
  http.begin(WEATHER_API);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    // 获取API响应内容
    const String payload = http.getString();

    // 解析天气JSON数据
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    // 提取并保存未来天气预报数据
    for (int i = 0; i < forecastDays; i++) {
      future.highTemp[i] = doc["data"]["forecast"][i]["high"];
      future.lowTemp[i] = doc["data"]["forecast"][i]["low"];
      future.weather[i] = doc["data"]["forecast"][i]["type"];
      future.fx[i] = doc["data"]["forecast"][i]["fx"];
      future.fl[i] = doc["data"]["forecast"][i]["fl"];
      future.week[i] = doc["data"]["forecast"][i]["week"];
      future.notice[i] = doc["data"]["forecast"][i]["notice"];
    }

    // 提取并解析日期信息
    const char *date = doc["date"];
    
    // 分割日期字符串为年、月、日
    strncpy(riqi.year, date, 4);
    riqi.year[4] = '\0';  // 添加字符串结束符

    strncpy(riqi.month, date + 4, 2);
    riqi.month[2] = '\0';  // 添加字符串结束符

    strncpy(riqi.day, date + 6, 2);
    riqi.day[2] = '\0';  // 添加字符串结束符
    
    // 重绘各部分显示内容
    drawTitle();   // 绘制顶部标题栏
    drawMiddle();  // 绘制中间天气信息
    //drawBottom();  // 绘制底部提示信息

    // 同步时间
    syncTime();
  }

  // 关闭HTTP连接
  http.end();
}

/**
 * @brief 主显示更新循环
 * 
 * 处理屏幕更新、时间同步、传感器数据更新和电池状态监控
 * 从主loop()抽取，使代码结构更清晰
 */
void updateMainDisplay() {
  unsigned long currentMillis = millis();
  static unsigned long lastDisplayUpdateMillis = 0; // 上次更新显示的时间
  static unsigned long lastBatteryUpdateMillis = 0; // 上次更新电池状态的时间
  static unsigned long lastHibernateMillis = 0;     // 上次休眠屏幕的时间
  static unsigned long lastLoopMillis = 0;          // 上次循环执行的时间
  static bool syncInProgress = false;               // 同步操作是否正在进行中
  
  // 循环执行频率控制，每50ms执行一次主逻辑，避免过度占用CPU
  if (currentMillis - lastLoopMillis < 50)
  {
    // 只更新按钮状态以确保按钮响应的及时性
    updateButtons();
    return;
  }
  lastLoopMillis = currentMillis;

  // 更新按钮状态
  updateButtons();
  
  // 检查是否需要联网同步时间和天气(每小时整点)
  if (needSync && !syncInProgress)
  {
    syncInProgress = true;  // 标记同步操作开始
    // Serial.println("开始执行整点联网同步...");

    // 提前确认是否需要连接WiFi
    bool needWiFi = true;
    
    // 只在需要时连接WiFi，避免不必要的连接
    if (needWiFi && !wifiConnected)
    {
      if (connectWiFi())
      {
        wifiConnected = true;
        // Serial.println("WiFi已连接以准备同步");
        
        // 清空屏幕并更新内容
        BW_refresh();

        // 初次更新时间和天气
        updateWeather();
        // Serial.println("天气数据已更新");
        syncTime();
        // Serial.println("NTP时间已同步");
        updateTime();
        updateSensorData(0); // 更新所有传感器数据

        // 更新时间戳并清除同步标记
        previousSyncMillis = currentMillis;
        needSync = false;
        
        // 同步完成后断开WiFi以节省电量
        disconnectWiFi();
        wifiConnected = false;
        // Serial.println("同步完成，已断开WiFi连接");
      }
      else
      {
        // WiFi连接失败处理，使用指数退避策略减少重试频率
        // Serial.println("WiFi连接失败，稍后重试同步");
        needSync = true;
        // 不使用阻塞延时，而是标记下次重试的时间
        static int retryCount = 0;
        static unsigned long nextRetryTime = 0;
        
        if (retryCount < 3) {
          // 前几次快速重试
          nextRetryTime = currentMillis + 30000; // 30秒后重试
          retryCount++;
        } else {
          // 之后延长重试间隔
          nextRetryTime = currentMillis + 300000; // 5分钟后重试
        }
        
        // 如果下次成功连接，重置重试计数
        if (currentMillis >= nextRetryTime) {
          syncInProgress = false;
        }
      }
    }
    
    syncInProgress = false;  // 标记同步操作结束
  }

  // 检查是否需要更新时间和传感器数据(每秒)
  if (currentMillis - previousTimeMillis >= timeInterval)
  {
    // 先更新时间
    updateTime();
    
    // 确保在更新完时间后立即更新传感器数据
    updateSensorData(0); // 更新所有传感器数据(0=全部更新)
    
    // 更新时间戳
    previousTimeMillis = currentMillis;
    

    // 每分钟更新一次电池状态，避免频繁读取
    if (currentMillis - lastBatteryUpdateMillis >= 60000)
    { 
      // 更新电池状态前确保WiFi是断开的，以获取准确的电池读数
      bool wifiWasConnected = (WiFi.status() == WL_CONNECTED);
      if (wifiWasConnected && !syncInProgress)
      {
        WiFi.disconnect(false); // false表示断开但保留配置
        delay(50);              // 缩短延时，减少阻塞时间
      }

      // 更新电池状态（强制更新）
      updateBatteryStatus(true);
      // Serial.println("已更新电池状态（每分钟）");

      // 如果之前WiFi是连接的并且需要重新连接
      if (wifiWasConnected && needSync && !syncInProgress)
      {
        WiFi.reconnect();
      }

      lastBatteryUpdateMillis = currentMillis;
    }
  }

  // 只在不需要频繁更新时休眠显示屏，以节省电力
  if (currentMillis - lastHibernateMillis >= 60000)  // 增加到60秒休眠一次
  { 
    // 检查是否有任何即将发生的操作需要活跃显示屏
    bool activityExpected = (currentMillis - previousSyncMillis >= syncInterval - 15000) || needSync;
    
    if (!activityExpected) {
      display.hibernate();
      lastHibernateMillis = currentMillis;
    }
  }
}
