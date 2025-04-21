/**
 * @file DisplayMain.ino
 * @brief 主显示管理，处理天气数据和屏幕布局
 * @author xiaocui
 * @date 2025-04-21
 * 
 * 此文件负责处理天气数据的获取、解析和显示，
 * 以及管理屏幕的主要布局和元素位置定义
 */

// ===== 布局常量定义 =====

// 未来天气详情相关位置
uint16_t day_x0 = 54;        // 未来天气详情X坐标

#define day_y0 90            // 未来天气详情Y坐标
#define day_y1 day_y0+18     // 第二行Y坐标
#define day_y2 day_y1+18     // 第三行Y坐标

// 图标相关位置
#define main_y0 14           // 更新时间图标Y坐标
#define main_y1 main_y0+19   // 位置图标Y坐标
#define main_y2 main_y1+19   // 天气状态图标Y坐标

#define main_x0 1            // 左边小图标X位置
#define main_x1 main_x0+15   // 左边小图标后的文字的X位置
#define main_x2 400-12       // 右边小图标X位置

// 实况温度位置
uint16_t temp_x;             // 温度X位置(动态计算)
#define temp_y0 50           // 温度Y位置

// 其他UI元素
uint16_t circle_x;           // 圈圈位置(动态计算)

/**
 * @brief 主显示处理函数
 * 
 * 负责管理主屏幕上的所有显示元素，
 * 检查是否需要更新天气和时间信息
 */
void display_main() {
  unsigned long currentMillis = millis();

  // 检查是否需要更新天气信息
  if (currentMillis - previousSyncMillis >= syncInterval && !needSync) {
    // 在主循环中已经处理同步，这里不需要重复操作
  }

  // 检查是否需要更新时间
  // 时间更新已在主循环中处理
}

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
    drawBottom();  // 绘制底部提示信息

    // 同步时间
    syncTime();
  }

  // 关闭HTTP连接
  http.end();
}
