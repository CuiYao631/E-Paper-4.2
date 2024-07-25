/*
 * 显示日期和天气图标的位置
*/


//绘制顶部 顶部数据包括 wifi信号，日期，电池电量
void drawTitle(String zf){
  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
  const char *character = zf.c_str();                            //String转换char
  uint16_t zf_width = u8g2Fonts.getUTF8Width(character);         //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);           //计算字符居中的X坐标（屏幕宽度/2-字符宽度/2）
  //display.setPartialWindow(0,  16, display.width(), 16);   //整行刷新
  display.setPartialWindow(0, 0, display.width(), 18); //设置局部刷新窗口
  display.firstPage();
  do
  {
    u8g2Fonts.setCursor(x, 0 * 16 + 13);
    u8g2Fonts.print(character);
    display.drawInvertedBitmap(375, 2, Bitmap_bat3, 21, 12, heise);
    display.drawLine(0, 17, 400, 17, 0); //顶部水平线
  }
  while (display.nextPage());
  
}

//绘制中间部分 中间这部分在时间下面，显示的是未来几天的天气
void drawMiddle(){
//  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
//  const char *character = zf.c_str();                            //String转换char
//  uint16_t zf_width = u8g2Fonts.getUTF8Width(character);         //获取字符的像素长度
//  uint16_t x = (display.width() / 2) - (zf_width / 2);           //计算字符居中的X坐标（屏幕宽度/2-字符宽度/2）

  display.setPartialWindow(0, 155, display.width(), 100); //设置局部刷新窗口
  display.firstPage();
  do
  {
//    u8g2Fonts.setCursor(x, 0 * 16 + 13);
//    u8g2Fonts.print(character);
//    display.drawInvertedBitmap(375, 2, Bitmap_bat3, 21, 12, heise);
    display.drawLine(0, 155, 400, 155, 0); //中间水平线

    //显示今天的天气

    //显示明天的天气

    //显示后天的天气


    
  }
  while (display.nextPage());
}


//绘制底部 底部还没有想好要显示啥
void drawBottom(){
  //  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
//  const char *character = zf.c_str();                            //String转换char
//  uint16_t zf_width = u8g2Fonts.getUTF8Width(character);         //获取字符的像素长度
//  uint16_t x = (display.width() / 2) - (zf_width / 2);           //计算字符居中的X坐标（屏幕宽度/2-字符宽度/2）

  display.setPartialWindow(0, 270, display.width(), 30); //设置局部刷新窗口
  display.firstPage();
  do
  {
//    u8g2Fonts.setCursor(x, 0 * 16 + 13);
//    u8g2Fonts.print(character);
//    display.drawInvertedBitmap(375, 2, Bitmap_bat3, 21, 12, heise);
    display.drawLine(0, 270, 400, 270, 0); //底部水平线
  }
  while (display.nextPage());
  
}
