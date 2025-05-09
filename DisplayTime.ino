
/**
 * 显示时间模块
 * 负责时间的获取、显示和同步
 */
#include <time.h>
// 使用config.h中定义的常量
#include "config.h"

// 获取并更新时间（局部刷新）
// 时间更新规则：每隔一秒获取一次RTC时间，如有变化则更新显示
void updateTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("无法获取时间");
    return;
  }

  RTC_minute = timeinfo.tm_min;
  RTC_hour = timeinfo.tm_hour;
  RTC_seconds = timeinfo.tm_sec;

  // 如果时间有变化就刷新时间（检查小时或分钟是否变化）
  if (RTC_hour != riqi.hours || RTC_minute != riqi.minutes)
  {
    Serial.println("时间发生变化，更新显示");
    drawTime(RTC_hour, RTC_minute, RTC_seconds);

    // 检查是否是整点（分钟为0），如果是则设置同步标志
    if (RTC_minute == 0)
    {
      extern bool needSync;
      needSync = true;
      Serial.println("整点到达，标记需要同步天气和时间");
    }
  }

  // 即使没有分钟变化，也需要更新内部时间状态
  riqi.hours = RTC_hour;
  riqi.minutes = RTC_minute;
  riqi.seconds = RTC_seconds;
}

// 设置时间到ESP32内置RTC
// 该函数需要在WiFi连接后调用
void setCustomTime(int year, int month, int day, int hour, int minute, int second)
{
  struct tm tm;
  tm.tm_year = year - 1900; // 从1900年开始
  tm.tm_mon = month - 1;    // 从0开始
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_sec = second;
  time_t t = mktime(&tm);
  struct timeval tv = {.tv_sec = t};
  settimeofday(&tv, NULL);
}

// 同步RTC时间
// 通过NTP获取当前时间并设置到RTC
// 该函数会在WiFi连接后调用
// 需要在setup()中调用WiFi.begin()连接WiFi
void syncTime()
{
  Serial.println("开始NTP时间同步...");

  // 检查WiFi连接状态
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi未连接，无法进行NTP同步");
    return;
  }

  // 安全初始化NTP客户端
  try
  {
    timeClient.begin();
    Serial.println("NTP客户端初始化成功");
  }
  catch (...)
  {
    Serial.println("NTP客户端初始化失败");
    return;
  }

  // 尝试更新NTP时间，增加超时和错误处理
  bool updateSuccess = false;
  int updateRetries = 0;
  const int maxUpdateRetries = 3;


  
  while (!updateSuccess && updateRetries < maxUpdateRetries)
  {
    Serial.print("尝试NTP更新，第");
    Serial.print(updateRetries + 1);
    Serial.println("次");

    try
    {
      updateSuccess = timeClient.update();
      if (updateSuccess)
      {
        Serial.println("NTP更新成功");
      }
      else
      {
        Serial.println("NTP更新返回失败状态");
        delay(1000); // 失败后等待1秒再重试
      }
    }
    catch (...)
    {
      Serial.println("NTP更新过程中出现异常");
    }

    updateRetries++;

    if (!updateSuccess && updateRetries < maxUpdateRetries)
    {
      delay(1000); // 重试前等待1秒
    }
  }

  if (!updateSuccess)
  {
    Serial.println("所有NTP更新尝试均失败，时间同步中止");
    return;
  }

  // 获取小时、分钟、秒
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("获取到epoch时间: ");
  Serial.println(epochTime);

  // 验证获取到的时间是否有效（不小于2025年1月1日）
  const unsigned long minValidEpoch = 1735689600; // 2025年1月1日的时间戳
  int retries = 0;
  const int maxRetries = 3;

  while (epochTime < minValidEpoch && retries < maxRetries)
  {
    Serial.println("获取到的时间无效，时间戳过小，重新获取...");
    delay(1000); // 等待1秒后重试
    timeClient.update();
    epochTime = timeClient.getEpochTime();
    Serial.print("重试获取epoch时间: ");
    Serial.println(epochTime);
    retries++;
  }

  if (epochTime < minValidEpoch)
  {
    Serial.println("警告：多次尝试后仍无法获取有效时间，使用最后一次获取的时间");
  }

  // 使用timeClient提供的方法获取时间组件
  riqi.hours = timeClient.getHours();
  riqi.minutes = timeClient.getMinutes();
  riqi.seconds = timeClient.getSeconds();

  // 调试输出获取到的时间
  Serial.print("NTP时间同步成功: ");
  Serial.print(riqi.hours);
  Serial.print(":");
  Serial.print(riqi.minutes);
  Serial.print(":");
  Serial.println(riqi.seconds);

  // 确认分钟值是否正确
  if (riqi.minutes == 0)
  {
    // 尝试直接从epoch计算分钟
    unsigned long totalMinutes = (epochTime / 60) % 60;
    Serial.print("从epoch计算的分钟值: ");
    Serial.println(totalMinutes);

    // 如果计算的分钟值有效且不为0，则使用计算值
    if (totalMinutes > 0 && totalMinutes < 60)
    {
      riqi.minutes = totalMinutes;
      Serial.print("使用计算的分钟值代替: ");
      Serial.println(riqi.minutes);
    }
  }

  // 获取完整日期（从timeClient的epoch时间）
  struct tm timeinfo;
  time_t t = epochTime;
  localtime_r(&t, &timeinfo);

  // 更新riqi结构体的年月日
  sprintf(riqi.year, "%04d", timeinfo.tm_year + 1900);
  sprintf(riqi.month, "%02d", timeinfo.tm_mon + 1);
  sprintf(riqi.day, "%02d", timeinfo.tm_mday);

  // 保存星期几到单独的变量，而不是覆盖日期
  const char *weekdays[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
  if (timeinfo.tm_wday >= 0 && timeinfo.tm_wday <= 6)
  {
    strcpy(riqi.weekday, weekdays[timeinfo.tm_wday]);
  }
  else
  {
    strcpy(riqi.weekday, "未知");
  }

  Serial.print("日期: ");
  Serial.print(riqi.year);
  Serial.print("/");
  Serial.print(riqi.month);
  Serial.print("/");
  Serial.print(riqi.day);
  Serial.print(" ");
  Serial.println(riqi.weekday);

  // 时间设置到RTC中
  setCustomTime(atoi(riqi.year), atoi(riqi.month), atoi(riqi.day), riqi.hours, riqi.minutes, riqi.seconds);
  drawTime(riqi.hours, riqi.minutes, riqi.seconds);
}

// 绘制时间
// 这里的时间绘制函数使用了局部刷新，避免了全屏刷新带来的闪烁
// 通过计算最大宽度来设置局部刷新窗口的大小
void drawTime(uint32_t RTC_hour, uint32_t RTC_minute, uint32_t RTC_seconds)
{
  String hour, minute, assembleTime;
  if (RTC_hour < 10)
    hour = "0" + String(RTC_hour);
  else
    hour = String(RTC_hour);
  if (RTC_minute < 10)
    minute = "0" + String(RTC_minute);
  else
    minute = String(RTC_minute);
  assembleTime = hour + ":" + minute;
  Serial.println(assembleTime);
  u8g2Fonts.setFont(u8g2_font_logisoso92_tn);

  // 计算最大宽度 - 使用模拟时间"88:88"来计算最大可能宽度
  const char *maxWidthTime = "88:88";
  uint16_t maxWidth = u8g2Fonts.getUTF8Width(maxWidthTime) + 7; // 最大宽度 + 偏移量

  int16_t fontAscent = u8g2Fonts.getFontAscent();   // 获取字体上升高度
  int16_t fontDescent = u8g2Fonts.getFontDescent(); // 获取字体下降高度
  int16_t zf_height = fontAscent - fontDescent;     // 计算字符高度

  // 设置固定大小的局部刷新窗口，使用最大可能宽度
  uint16_t refreshWidth = maxWidth; // 使用固定的最大宽度
  uint16_t refreshHeight = fontAscent - fontDescent - 7;

  // 设置局部刷新窗口
  display.setPartialWindow(TIME_X, TIME_Y - fontAscent, refreshWidth, refreshHeight);

  // 使用更强的刷新模式（可选）
  // display.setDisplayMode(DISPLAY_MODE_ACCURATE_REFRESH); // 如果支持此模式

  display.firstPage();
  do
  {
    // 先清除区域（可选，取决于屏幕特性）
    display.fillRect(TIME_X, TIME_Y - fontAscent, refreshWidth, refreshHeight, GxEPD_WHITE);

    // 绘制新的时间文本
    u8g2Fonts.setCursor(TIME_X, TIME_Y);
    u8g2Fonts.print(assembleTime);
  } while (display.nextPage());

  riqi.hours = RTC_hour;
  riqi.minutes = RTC_minute;
}
