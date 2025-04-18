
/*
 * 显示时间 
*/
#include <time.h> // 添加time.h头文件以支持time_t和gmtime
#define time_x 66 //时间位置X
#define time_y 123 //时间位置Y


//更新时间
//时间更新规则为：每隔一秒获取一次rtc时间，如果时间发生变化则更新显示时间,以保证时间的准确性
//获取并更新时间（局部刷新）
void updateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("无法获取时间");
    return;
  }

  RTC_minute = timeinfo.tm_min;
  RTC_hour = timeinfo.tm_hour;
  RTC_seconds = timeinfo.tm_sec;

  //如果时间有变化就刷新时间（检查小时或分钟是否变化）
  if(RTC_hour != riqi.hours || RTC_minute != riqi.minutes) {
    Serial.println("时间发生变化，更新显示");
    drawTime(RTC_hour, RTC_minute, RTC_seconds);
  }
  
  // 即使没有分钟变化，也需要更新内部时间状态
  riqi.hours = RTC_hour;
  riqi.minutes = RTC_minute;
  riqi.seconds = RTC_seconds;
}

//设置时间到自带RTC
void setCustomTime(int year, int month, int day, int hour, int minute, int second) {
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

//同步RTC时间
void syncTime() {
  Serial.println("开始NTP时间同步...");
  
  // 确保NTP客户端有足够时间连接到服务器
  timeClient.begin(); 
  

    
  timeClient.update();

  
  // 获取小时、分钟、秒
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("获取到epoch时间: ");
  Serial.println(epochTime);
  
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
  if (riqi.minutes == 0) {
    // 尝试直接从epoch计算分钟
    unsigned long totalMinutes = (epochTime / 60) % 60;
    Serial.print("从epoch计算的分钟值: ");
    Serial.println(totalMinutes);
    
    // 如果计算的分钟值有效且不为0，则使用计算值
    if (totalMinutes > 0 && totalMinutes < 60) {
      riqi.minutes = totalMinutes;
      Serial.print("使用计算的分钟值代替: ");
      Serial.println(riqi.minutes);
    }
  }
  
  // 获取完整日期（从timeClient的epoch时间）
  struct tm timeinfo;
  localtime_r((time_t*)&epochTime, &timeinfo);
  
  // 更新riqi结构体的年月日
  sprintf(riqi.year, "%04d", timeinfo.tm_year + 1900);
  sprintf(riqi.month, "%02d", timeinfo.tm_mon + 1);
  sprintf(riqi.day, "%02d", timeinfo.tm_mday);
  
  // 根据星期几设置day值
  switch(timeinfo.tm_wday) {
    case 0: strcpy(riqi.day, "星期日"); break;
    case 1: strcpy(riqi.day, "星期一"); break;
    case 2: strcpy(riqi.day, "星期二"); break;
    case 3: strcpy(riqi.day, "星期三"); break;
    case 4: strcpy(riqi.day, "星期四"); break;
    case 5: strcpy(riqi.day, "星期五"); break;
    case 6: strcpy(riqi.day, "星期六"); break;
  }
  
  Serial.print("日期: ");
  Serial.print(riqi.year);
  Serial.print("/");
  Serial.print(riqi.month);
  Serial.print("/");
  Serial.print(riqi.day);
  Serial.print(" ");
  Serial.println(riqi.day);

  // 时间设置到RTC中
  setCustomTime(atoi(riqi.year), atoi(riqi.month), atoi(riqi.day), riqi.hours, riqi.minutes, riqi.seconds);
  drawTime(riqi.hours, riqi.minutes, riqi.seconds);
}
void drawTime(uint32_t RTC_hour,  uint32_t RTC_minute,uint32_t RTC_seconds){
   String hour, minute, assembleTime;
    if (RTC_hour < 10)   hour = "0" + String(RTC_hour);
    else                 hour = String(RTC_hour);
    if (RTC_minute < 10) minute = "0" + String(RTC_minute);
    else                 minute = String(RTC_minute);
    assembleTime = hour + ":" + minute;
    Serial.println(assembleTime);
    u8g2Fonts.setFont(u8g2_font_logisoso92_tn);
    const char *character = assembleTime.c_str();
    
    uint16_t zf_width = u8g2Fonts.getUTF8Width(character)+7; // 获取字符的像素宽度 //添加X轴偏移量
    int16_t fontAscent = u8g2Fonts.getFontAscent(); // 获取字体上升高度
    int16_t fontDescent = u8g2Fonts.getFontDescent(); // 获取字体下降高度
    int16_t zf_height = fontAscent - fontDescent; // 计算字符高度
  
    //uint16_t x = (display.width() / 2) - (zf_width / 2); // 计算字符居中的X坐标（屏幕宽度/2 - 字符宽度/2）
    //int16_t y = ((display.height() / 2) + (zf_height / 2)/2); // 计算字符居中的Y坐标（屏幕高度/2 + 字符高度/2）

    // y=y-55;//向上偏移
  
    // 设置局部刷新窗口，窗口的高度应该是字体的高度 
    display.setPartialWindow(time_x, time_y - fontAscent, zf_width, fontAscent - fontDescent-7);
    display.firstPage();
    do {
      u8g2Fonts.setCursor(time_x, time_y);
      u8g2Fonts.print(assembleTime);
    } while (display.nextPage());
    
    riqi.hours=RTC_hour;
    riqi.minutes=RTC_minute;
}
