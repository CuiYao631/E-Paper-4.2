/*
 * 显示日期和天气图标的位置
*/

#define title_y 28 //顶部位置Y
#define middle_y 140 //中间部分位置Y


//绘制顶部 顶部数据包括 wifi信号，日期，电池电量
void drawTitle(){
  String currentDate = String(riqi.year) + "-" + String(riqi.month) + "-" + String(riqi.day);
  
  // 为日期单独使用大一号字体，使其更醒目
  u8g2Fonts.setFont(u8g2_font_inb16_mf);  // 使用18px字体来显示日期
  const char *character = currentDate.c_str();                            //String转换char
  uint16_t zf_width = u8g2Fonts.getUTF8Width(character);         //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);           //计算字符居中的X坐标（屏幕宽度/2-字符宽度/2）
  
  //display.setPartialWindow(0,  16, display.width(), 16);   //整行刷新
  display.setPartialWindow(0, 0, display.width(), title_y); //设置局部刷新窗口
  display.firstPage();
  do
  {
    //位置小图标
    display.drawInvertedBitmap(5, 7, Bitmap_weizhi, 13, 13, heise);
    
    // 位置信息使用小号字体
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);  // 使用15px字体来显示位置信息
    u8g2Fonts.setCursor(20, 18);
    u8g2Fonts.print("西安");

    // 切换到大字体显示日期
    u8g2Fonts.setFont(u8g2_font_inb16_mf); 
    u8g2Fonts.setCursor(x, 20);  // 适当调整Y坐标以适应新字体
    u8g2Fonts.print(character);
    
    // 显示电池图标和电量，使用小号字体
    display.drawInvertedBitmap(375, 7, Bitmap_bat3, 21, 12, heise);
    
    // 使用小号字体显示电池电量
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
    String batteryText = String(getBatteryPercentage()) + "%";
    u8g2Fonts.setCursor(345, 18);
    u8g2Fonts.print(batteryText);
    
    display.drawLine(0, title_y-1, 400, title_y-1, 0); //顶部水平线
  }
  while (display.nextPage());
  
}

//绘制中间部分 中间这部分在时间下面，显示的是未来几天的天气
void drawMiddle(){
  // 定义每个天气栏的X坐标位置
  const uint16_t xPositions[5] = {25, 105, 185, 255, 325};
  
  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
  display.setPartialWindow(0, 140, display.width(), 140); //设置局部刷新窗口
  display.firstPage();
  do
  {
    display.drawLine(0, 140, 400, 140, 0); //中间水平线
    
    //日期标签
    const char* dayLabels[3] = {"今天", "明天", "后天"};
    
    // 渲染日期标签
    for(uint8_t i = 0; i < 5; i++) {
      u8g2Fonts.setCursor(xPositions[i], 158);
      if(i < 3) {
        u8g2Fonts.print(dayLabels[i]);
      } else {
        u8g2Fonts.print(future.week[i]);
      }
      
      // 天气图标和文字
      drawWeatherBitmap(xPositions[i]-5, String(future.weather[i]));
      
      // 温度
      drawtemperature(xPositions[i]-6, String(future.highTemp[i]), String(future.lowTemp[i]));
    }
  }
  while (display.nextPage());
}


//绘制底部 底部还没有想好要显示啥
void drawBottom(){

  const char *character =String(future.notice[0]).c_str();                            //String转换char
  uint16_t zf_width = u8g2Fonts.getUTF8Width(character);         //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);           //计算字符居中的X坐标（屏幕宽度/2-字符宽度/2）

  display.setPartialWindow(0, 280, display.width(), 20); //设置局部刷新窗口
  display.firstPage();
  do
  {
    display.drawLine(0, 280, 400, 280, 0); //底部水平线
    u8g2Fonts.setCursor(x, 298);
    u8g2Fonts.print(String(future.notice[0]));
  }
  while (display.nextPage());
  
}

//显示天气图标和文字
void drawWeatherBitmap(uint16_t x, String type) {
  // 天气图标和文字对齐偏移表
  struct WeatherIconMapping {
    const char* weatherType;
    const uint8_t* bitmap;
    int8_t textOffsetX;
  };
  
  // 定义天气图标映射关系
  static const WeatherIconMapping weatherIcons[] = {
    {"", Bitmap_wz, 0},         // 未知天气
    {"晴", Bitmap_qt, 15},
    {"多云", Bitmap_yt, 5},
    {"阴", Bitmap_dy, 15},
    {"小雨", Bitmap_xy, 5},
    {"小到中雨", Bitmap_zhongy, 0},
    {"中雨", Bitmap_zhongy, 5},
    {"中到大雨", Bitmap_dayu, 0},
    {"大雨", Bitmap_dayu, 5},
    {"大到暴雨", Bitmap_by, 0},
    {"暴雨", Bitmap_by, 5},
    {"暴雨到大暴雨", Bitmap_dby, 0},
    {"大暴雨", Bitmap_dby, 0},
    {"大暴雨到特大暴雨", Bitmap_tdby, 0},
    {"特大暴雨", Bitmap_tdby, 0},
    {"冻雨", Bitmap_dongy, 5},
    {"阵雨", Bitmap_zheny, 5},
    {"雷阵雨", Bitmap_lzy, 0},
    {"雨夹雪", Bitmap_yjx, 0},
    {"雷阵雨伴有冰雹", Bitmap_lzybbb, 0},
    {"小雪", Bitmap_xx, 5},
    {"小到中雪", Bitmap_zhongx, 0},
    {"中雪", Bitmap_zhongx, 5},
    {"中到大雪", Bitmap_dx, 0},
    {"大雪", Bitmap_dx, 5},
    {"大到暴雪", Bitmap_dx, 5},
    {"暴雪", Bitmap_dx, 5},
    {"阵雪", Bitmap_dx, 5},
    {"强沙尘暴", Bitmap_scb, 0},
    {"扬沙", Bitmap_ys, 5},
    {"沙尘暴", Bitmap_scb, 0},
    {"浮尘", Bitmap_fc, 5},
    {"雾", Bitmap_w, 15},
    {"霾", Bitmap_m, 15}
  };
  
  // 默认使用未知天气图标
  const uint8_t* iconBitmap = Bitmap_wz;
  int8_t textOffset = 0;
  
  // 查找匹配的天气类型
  const int numWeatherTypes = sizeof(weatherIcons) / sizeof(WeatherIconMapping);
  for (int i = 0; i < numWeatherTypes; i++) {
    if (type == weatherIcons[i].weatherType) {
      iconBitmap = weatherIcons[i].bitmap;
      textOffset = weatherIcons[i].textOffsetX;
      break;
    }
  }
  
  // 绘制天气图标
  display.drawInvertedBitmap(x, 170, iconBitmap, 45, 45, heise);
  
  // 绘制天气文字
  if (type != "") {
    u8g2Fonts.setCursor(x + textOffset, 225);
    u8g2Fonts.print(type);
  }
}
//显示最高温度和最低温度
void drawtemperature(uint16_t x, String highTemp, String lowTemp) {
  // 提取并显示最高温度
  int highdelimiterIndex = highTemp.indexOf(" "); // 查找分隔符的位置
  String hightemperature = "高 ";
  
  // 仅当找到分隔符才进行子字符串提取
  if (highdelimiterIndex != -1) {
    hightemperature += highTemp.substring(highdelimiterIndex + 1); // 获取分隔符后的子字符串
  } else {
    hightemperature += highTemp; // 如果没有分隔符，使用完整字符串
  }
  
  u8g2Fonts.setCursor(x, 250);
  u8g2Fonts.print(hightemperature);
  
  // 提取并显示最低温度
  int lowdelimiterIndex = lowTemp.indexOf(" "); // 查找分隔符的位置
  String lowtemperature = "低 ";
  
  // 仅当找到分隔符才进行子字符串提取
  if (lowdelimiterIndex != -1) {
    lowtemperature += lowTemp.substring(lowdelimiterIndex + 1); // 获取分隔符后的子字符串
  } else {
    lowtemperature += lowTemp; // 如果没有分隔符，使用完整字符串
  }
  
  u8g2Fonts.setCursor(x, 275);
  u8g2Fonts.print(lowtemperature);
}

