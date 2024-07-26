/*
 * 显示日期和天气图标的位置
*/

#define title_y 22 //顶部位置Y
#define middle_y 140 //中间部分位置Y


//绘制顶部 顶部数据包括 wifi信号，日期，电池电量
void drawTitle(){
  String currentDate = String(riqi.year) + "-" + String(riqi.month) + "-" + String(riqi.day);
  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
  const char *character = currentDate.c_str();                            //String转换char
  uint16_t zf_width = u8g2Fonts.getUTF8Width(character);         //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);           //计算字符居中的X坐标（屏幕宽度/2-字符宽度/2）
  //display.setPartialWindow(0,  16, display.width(), 16);   //整行刷新
  display.setPartialWindow(0, 0, display.width(), title_y); //设置局部刷新窗口
  display.firstPage();
  do
  {
    //位置小图标
    display.drawInvertedBitmap(5, 5, Bitmap_weizhi, 13, 13, heise);
    u8g2Fonts.setCursor(20, 17);
    u8g2Fonts.print("西安");

    u8g2Fonts.setCursor(x, 17);
    u8g2Fonts.print(character);
    
    display.drawInvertedBitmap(375, 3, Bitmap_bat3, 21, 12, heise);
    display.drawLine(0, title_y-1, 400, title_y-1, 0); //顶部水平线
  }
  while (display.nextPage());
  
}

//绘制中间部分 中间这部分在时间下面，显示的是未来几天的天气
void drawMiddle( ){
  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
//  const char *character = zf.c_str();                            //String转换char
//  uint16_t zf_width = u8g2Fonts.getUTF8Width(character);         //获取字符的像素长度
//  uint16_t x = (display.width() / 2) - (zf_width / 2);           //计算字符居中的X坐标（屏幕宽度/2-字符宽度/2）

  display.setPartialWindow(0, 140, display.width(), 140); //设置局部刷新窗口
  display.firstPage();
  do
  {
//    u8g2Fonts.setCursor(x, 0 * 16 + 13);
//    u8g2Fonts.print(character);
//    display.drawInvertedBitmap(375, 2, Bitmap_bat3, 21, 12, heise);
    display.drawLine(0, 140, 400, 140, 0); //中间水平线
    
    //display.drawLine(0, 140, 400, 140, 0); //中间水平线
    
    //日期
    u8g2Fonts.setCursor(30, 158);
    u8g2Fonts.print("今天");
    u8g2Fonts.setCursor(110, 158);
    u8g2Fonts.print("明天");
    u8g2Fonts.setCursor(190, 158);
    u8g2Fonts.print("后天");
    u8g2Fonts.setCursor(260, 158);
    u8g2Fonts.print(future.week[3]);
    u8g2Fonts.setCursor(330, 158);
    u8g2Fonts.print(future.week[4]);

    //天气
    drawWeatherBitmap(30-5,String(future.weather[0]));
    drawWeatherBitmap(110-5,String(future.weather[1]));
    drawWeatherBitmap(190-5,String(future.weather[2]));
    drawWeatherBitmap(260-5,String(future.weather[3]));
    drawWeatherBitmap(330-5,String(future.weather[4]));

    //温度
    drawtemperature(30-6 ,String(future.highTemp[0]),String(future.lowTemp[0]));
    drawtemperature(110-6 ,String(future.highTemp[1]),String(future.lowTemp[1]));
    drawtemperature(190-6 ,String(future.highTemp[2]),String(future.lowTemp[2]));
    drawtemperature(260-6 ,String(future.highTemp[3]),String(future.lowTemp[3]));
    drawtemperature(330-6 ,String(future.highTemp[4]),String(future.lowTemp[4]));
    
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
void drawWeatherBitmap(uint16_t x,String type){
  if (type==""){ display.drawInvertedBitmap(x, 170, Bitmap_wz, 45, 45, heise);}
  else if (type=="晴") { display.drawInvertedBitmap(x, 170, Bitmap_qt, 45, 45, heise);u8g2Fonts.setCursor(x+15, 225);u8g2Fonts.print(type);}
  else if (type=="多云"){  display.drawInvertedBitmap(x, 170, Bitmap_yt, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="阴"){  display.drawInvertedBitmap(x, 170, Bitmap_dy, 45, 45, heise);u8g2Fonts.setCursor(x+15, 225);u8g2Fonts.print(type);}
  else if (type=="小雨") { display.drawInvertedBitmap(x, 170, Bitmap_xy, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="小到中雨") { display.drawInvertedBitmap(x, 170, Bitmap_zhongy, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="中雨"){  display.drawInvertedBitmap(x, 170, Bitmap_zhongy, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="中到大雨") { display.drawInvertedBitmap(x, 170, Bitmap_dayu, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="大雨"){  display.drawInvertedBitmap(x, 170, Bitmap_dayu, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="大到暴雨") { display.drawInvertedBitmap(x, 170, Bitmap_by, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="暴雨") { display.drawInvertedBitmap(x, 170, Bitmap_by, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="暴雨到大暴雨"){  display.drawInvertedBitmap(x, 170, Bitmap_dby, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="大暴雨") { display.drawInvertedBitmap(x, 170, Bitmap_dby, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="大暴雨到特大暴雨") { display.drawInvertedBitmap(x, 170, Bitmap_tdby, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="特大暴雨") { display.drawInvertedBitmap(x, 170, Bitmap_tdby, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="冻雨") { display.drawInvertedBitmap(x, 170, Bitmap_dongy, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="阵雨"){  display.drawInvertedBitmap(x, 170, Bitmap_zheny, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="雷阵雨") { display.drawInvertedBitmap(x, 170, Bitmap_lzy, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="雨夹雪"){  display.drawInvertedBitmap(x, 170, Bitmap_yjx, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="雷阵雨伴有冰雹") { display.drawInvertedBitmap(x, 170, Bitmap_lzybbb, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="小雪") { display.drawInvertedBitmap(x, 170, Bitmap_xx, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="小到中雪") { display.drawInvertedBitmap(x, 170, Bitmap_zhongx, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="中雪"){  display.drawInvertedBitmap(x, 170, Bitmap_zhongx, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="中到大雪") { display.drawInvertedBitmap(x, 170, Bitmap_dx, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="大雪") { display.drawInvertedBitmap(x, 170, Bitmap_dx, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="大到暴雪"){  display.drawInvertedBitmap(x, 170, Bitmap_dx, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="暴雪"){  display.drawInvertedBitmap(x, 170, Bitmap_dx, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="阵雪"){  display.drawInvertedBitmap(x, 170, Bitmap_dx, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="强沙尘暴"){  display.drawInvertedBitmap(x, 170, Bitmap_scb, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="扬沙"){  display.drawInvertedBitmap(x, 170, Bitmap_ys, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="沙尘暴") { display.drawInvertedBitmap(x, 170, Bitmap_scb, 45, 45, heise);u8g2Fonts.setCursor(x, 225);u8g2Fonts.print(type);}
  else if (type=="浮尘"){  display.drawInvertedBitmap(x, 170, Bitmap_fc, 45, 45, heise);u8g2Fonts.setCursor(x+5, 225);u8g2Fonts.print(type);}
  else if (type=="雾") { display.drawInvertedBitmap(x, 170, Bitmap_w, 45, 45, heise);u8g2Fonts.setCursor(x+15, 225);u8g2Fonts.print(type);}
  else if (type=="霾") { display.drawInvertedBitmap(x, 170, Bitmap_m, 45, 45, heise);u8g2Fonts.setCursor(x+15, 225);u8g2Fonts.print(type);}
}
//显示最高温度和最低温度
void drawtemperature(uint16_t x,String highTemp,String lowTemp){

    u8g2Fonts.setCursor(x, 250);
    int highdelimiterIndex = highTemp.indexOf(" "); // 查找分隔符的位置
    String hightemperature ="高 "+ highTemp.substring(highdelimiterIndex + 1); // 获取分隔符后的子字
    u8g2Fonts.print(hightemperature);
    u8g2Fonts.setCursor(x, 275);
    int lowdelimiterIndex = lowTemp.indexOf(" "); // 查找分隔符的位置
    String lowtemperature ="低 "+lowTemp.substring(lowdelimiterIndex + 1); // 获取分隔符后的子字
    u8g2Fonts.print(lowtemperature);
}

