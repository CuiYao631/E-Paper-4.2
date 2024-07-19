

SPISettings spi_settings(4000000, MSBFIRST, SPI_MODE0);
#define dc 0
#define cs 15
void EPD_writeCommand(uint8_t c)
{
  SPI.beginTransaction(spi_settings);
  if (dc >= 0) digitalWrite(dc, LOW);  //dc
  if (cs >= 0) digitalWrite(cs, LOW);  //cs
  SPI.transfer(c);
  if (dc >= 0) digitalWrite(dc, HIGH);   //dc
  if (cs >= 0) digitalWrite(cs, HIGH);   //cs
  SPI.endTransaction();
}
void EPD_writeData(uint8_t d)
{
  SPI.beginTransaction(spi_settings);
  if (cs >= 0) digitalWrite(cs, LOW); //cs
  SPI.transfer(d);
  if (cs >= 0) digitalWrite(cs, HIGH); //cs
  SPI.endTransaction();
}
void xiaobian() //消除黑边（四周的边跟随屏幕刷新，仅全局刷新有效）
{
  EPD_writeCommand(0x3c);  // 边界波形控制寄存器
  EPD_writeData(0x33);     // 向里面写入数据

  //EPD_writeCommand(0x2c); // VCOM setting
  //EPD_writeData(0xA1);    // * different   FPC丝印A1 库默认A8
}

void BW_refresh() //黑白刷新一次
{
  display.init(0, 0, 10, 0);
  display.fillScreen(heise);  // 填充屏幕
  display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲
  display.fillScreen(baise);
  display.display(1);
}
////绘制矩形
//void DrawRectangle(){
//  // 设置矩形的坐标和大小
//  int16_t x = 0;
//  int16_t y = 0;
//  int16_t width = 400;
//  int16_t height = 20;
//
//  // 设置矩形的颜色
//  uint16_t color = GxEPD_BLACK;
//
//  // 绘制矩形
//  display.drawRect(x, y, width, height, color);
//
//
//  // 更新显示
//  display.display();
//}
//
////绘制圆形
//void DrawCircle(){
//  int16_t x = display.width() / 2; // 屏幕中心点的 x 坐标
//  int16_t y = display.height() / 2; // 屏幕中心点的 y 坐标
//  int16_t radius = 30; // 圆形的半径
//
//  // 设置圆形的颜色
//  uint16_t color = GxEPD_BLACK;
//
//  // 绘制圆形
//  display.drawCircle(x, y, radius, color);
//
//  // 更新显示
//  display.display(true);
//   // 等待一段时间
//  delay(5000);
//}

//显示图片
void DrawImage(){
  
  // 绘制图片
  display.drawBitmap(0, 0, imageBitmap, display.width(), display.height(), GxEPD_BLACK);
  //display.drawBitmap(60, 60, rili, 60, 60, GxEPD_BLACK);

  // 更新显示
  display.display();
}
//绘制开机进度条
void onProgressBar(int progress,int x, char* str){
// 设置进度条背景颜色
  uint16_t backgroundColor = GxEPD_WHITE;
  // 绘制进度条背景
  display.drawRect(progressBarX, progressBarY, progressBarWidth, progressBarHeight, GxEPD_BLACK);

  // 设置进度条填充颜色
  uint16_t progressColor = GxEPD_BLACK;
  // 计算填充矩形的宽度
  int16_t progressWidth = map(progress, 0, 100, 0, progressBarWidth);
  // 绘制进度条填充矩形
  display.fillRect(progressBarX, progressBarY, progressWidth, progressBarHeight, progressColor);

  //设置开机加载文字
  u8g2Fonts.setCursor(x, progressBarY+35);
  display.fillRect(progressBarX, progressBarY+20, progressBarWidth, progressBarHeight, GxEPD_WHITE);
  u8g2Fonts.println(str);
  // 更新显示
  display.display(true);

  // 等待一段时间
  delay(200);
}


//发送局部刷新的显示信息到屏幕,不带居中
void display_partialLineNoCenter(uint8_t line, String zf) 
{
  display.setPartialWindow(0, line * 16, display.width(), 16);  //整行刷新
  display.firstPage();
  do
  {
    u8g2Fonts.setCursor(2, line * 16 + 13);
    u8g2Fonts.print(zf);
  }
  while (display.nextPage());
  //display.powerOff(); //关闭屏幕电源
}


 ////发送局部刷新的显示信息到屏幕,带居中
void display_partialLine(uint8_t line, String zf)
{
  /*
    display_partialLine()
    发送局部刷新的显示信息到屏幕,带居中

    line        行数（0-7）
    zf          字符内容
    lineRefresh 整行刷新 1-是 0-仅刷新字符长度的区域
  */
  //u8g2Fonts.setFont(chinese_gb2312);
  const char *character = zf.c_str();                            //String转换char
  uint16_t zf_width = u8g2Fonts.getUTF8Width(character);         //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);           //计算字符居中的X坐标（屏幕宽度/2-字符宽度/2）
  display.setPartialWindow(0, line * 16, display.width(), 16);   //整行刷新
  display.firstPage();
  do
  {
    u8g2Fonts.setCursor(x, line * 16 + 13);
    u8g2Fonts.print(character);
  }
  while (display.nextPage());
  //display.powerOff(); //关闭屏幕电源
}
