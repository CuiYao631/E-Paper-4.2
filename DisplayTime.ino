
/*
 * 显示时间 
*/
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

  //如果时间有变化就刷新时间
  if(RTC_hour!=riqi.hours|| RTC_minute!=riqi.minutes){
   
   drawTime(RTC_hour, RTC_minute,RTC_seconds);
  }
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
  // 获取NTP时间
  timeClient.update();
   
  riqi.hours = timeClient.getHours();
  riqi.minutes = timeClient.getMinutes();
  riqi.seconds = timeClient.getSeconds();

  Serial.print(riqi.hours);
  Serial.print(":");
  Serial.print(riqi.minutes);
  Serial.print(":");
  Serial.println(riqi.seconds);
  

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
