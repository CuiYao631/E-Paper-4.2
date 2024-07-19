//3天情况的位置

//带温湿度的位置
uint16_t day_x0 = 54;        //未来天气详情X坐标

#define day_y0 90  //未来天气详情Y坐标
#define day_y1 day_y0+18
#define day_y2 day_y1+18
//小提示图标位置
#define main_y0 14         //更新时间图标
#define main_y1 main_y0+19   //位置图标
#define main_y2 main_y1+19   //天气状态图标

#define main_x0 1         // 左边小图标X位置
#define main_x1 main_x0+15  // 左边小图标后的文字的X位置

#define main_x2 400-12    // 右边小图标X位置
//实况温度位置
uint16_t temp_x;
#define temp_y0 50
//圈圈位置
uint16_t circle_x;

void display_main()
{
  //u8g2Fonts.setFontMode(1);
  display.setFullWindow(); //设置全屏刷新 height
  display.firstPage();
  do
  {
    //display.setPartialWindow(0, 0, display.width(), display.height()); //设置局部刷新窗口

    //display.fillScreen(heise);  // 填充屏幕
    //display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲
    //display.fillScreen(baise);  // 填充屏幕
    //display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲

    /* u8g2Fonts.setCursor(275, 16); //实时时间-小时
      u8g2Fonts.print(RTC_hour);
      u8g2Fonts.setCursor(275, 32); //实时时间-小时
      u8g2Fonts.print(RTC_night_count);*/

    //****** 显示实况温度和天气图标 ******
    //提取最后更新时间的 仅提取 小时:分钟
    String minutes_hours;
    for (uint8_t i = 11; i < 16; i++) minutes_hours += actual.last_update[i];

    const char* minutes_hours_c = minutes_hours.c_str();                         //String转换char
    uint16_t minutes_hours_length = u8g2Fonts.getUTF8Width(minutes_hours_c);     //获取最后更新时间的长度
    uint16_t city_length = u8g2Fonts.getUTF8Width(actual.city);                  //获取城市的字符长度
    uint16_t weather_name_length = u8g2Fonts.getUTF8Width(actual.weather_name);  //获取天气现象的字符长度
    uint16_t uvi_x = 284 - (strlen(life_index.uvi) * 26 / 6 + 3);                //UVI字符的位置

    //计算这三个数值谁最大
    int num[3] = {minutes_hours_length, city_length, weather_name_length};
    int len = sizeof(num) / sizeof(*num);
    int sy_length = uvi_x + (get_max_num(num, len) + main_x1); //剩余长度

    u8g2Fonts.setFont(u8g2_font_fub42_tn); //实况温度用字体

    int temp_x_length = u8g2Fonts.getUTF8Width(actual.temp) + 20 + 58;
    temp_x = (sy_length / 2) - (temp_x_length / 2);
    u8g2Fonts.setCursor(temp_x, temp_y0);  //显示实况温度
    u8g2Fonts.print(actual.temp);

    //画圆圈
    circle_x = temp_x + u8g2Fonts.getUTF8Width(actual.temp) + 8; //计算圈圈的位置
    display.drawCircle(circle_x, temp_y0 - 34, 3, 0);
    display.drawCircle(circle_x, temp_y0 - 34, 4, 0);
    //显示天气图标
    display_tbpd();

    //****** 显示小图标和详情信息 ******
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
    //最后更新时间
    display.drawInvertedBitmap(main_x0, main_y0 - 12, Bitmap_gengxing, 13, 13, heise); //画最后更新时间小图标
    u8g2Fonts.setCursor(main_x1, main_y0);
    u8g2Fonts.print(minutes_hours);
    //城市名
    display.drawInvertedBitmap(main_x0, main_y1 - 12, Bitmap_weizhi, 13, 13, heise); //画位置小图标
    u8g2Fonts.setCursor(main_x1, main_y1);
    u8g2Fonts.print(actual.city);
    //天气实况状态
    display.drawInvertedBitmap(main_x0, main_y2 - 12, Bitmap_zhuangtai, 13, 13, heise); //画天气状态小图标
    u8g2Fonts.setCursor(main_x1, main_y2);
    u8g2Fonts.print(actual.weather_name);
    //紫外线指数
    u8g2Fonts.setFont(u8g2_font_u8glib_4_tf);
    u8g2Fonts.setCursor(284, main_y0 - 3);
    u8g2Fonts.print("UVI");
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a);
    u8g2Fonts.setCursor(uvi_x, main_y0);
    u8g2Fonts.print(life_index.uvi);
    //Serial.print("life_index.uvi长度："); Serial.println(strlen(life_index.uvi));
    //Serial.print("life_index.uvi："); Serial.println(life_index.uvi);
    //湿度
    display.drawInvertedBitmap(main_x2, main_y1 - 12, Bitmap_humidity, 13, 13, heise);
    u8g2Fonts.setCursor(main_x2 - (strlen(future.date0_humidity) * 6 + 3), main_y1);
    u8g2Fonts.print(future.date0_humidity);
    //风力等级
    display.drawInvertedBitmap(main_x2, main_y2 - 12, Bitmap_fx, 13, 13, heise);
    u8g2Fonts.setCursor(main_x2 - (strlen(future.date0_wind_scale) * 6 + 3), main_y2);
    u8g2Fonts.print(future.date0_wind_scale);

    //************ 显示未来天气 ************
    //拼装月日字符串 格式02-02
    String day0, day1, day2;
    for (uint8_t i = 5; i < 10; i++)
    {
      day0 += future.date0[i];
      day1 += future.date1[i];
      day2 += future.date2[i];
    }

    //拼装星期几
    //提取年月日并转换成int
    String nian0, nian1, nian2, yue0, yue1, yue2, ri0, ri1, ri2;
    int nian0_i, nian1_i, nian2_i, yue0_i, yue1_i, yue2_i, ri0_i, ri1_i, ri2_i;
    for (uint8_t i = 0; i <= 9; i++)
    {
      if (i <= 3)
      {
        nian0 += future.date0[i];
        nian1 += future.date1[i];
        nian2 += future.date2[i];
      }
      else if (i == 5 || i == 6)
      {
        yue0 += future.date0[i];
        yue1 += future.date1[i];
        yue2 += future.date2[i];
      }
      else if (i == 8 || i == 9)
      {
        ri0 += future.date0[i];
        ri1 += future.date1[i];
        ri2 += future.date2[i];
      }
    }
    nian0_i = atoi(nian0.c_str()); yue0_i = atoi(yue0.c_str()); ri0_i = atoi(ri0.c_str());
    nian1_i = atoi(nian1.c_str()); yue1_i = atoi(yue1.c_str()); ri1_i = atoi(ri1.c_str());
    nian2_i = atoi(nian2.c_str()); yue2_i = atoi(yue2.c_str()); ri2_i = atoi(ri2.c_str());

    //Serial.print("年:"); Serial.print(nian0_i); Serial.print(" "); Serial.print(nian1_i); Serial.print(" "); Serial.println(nian2_i);
    //Serial.print("月:"); Serial.print(yue0_i); Serial.print(" "); Serial.print(yue1_i); Serial.print(" "); Serial.println(yue2_i);
    //Serial.print("日:"); Serial.print(ri0_i); Serial.print(" "); Serial.print(ri1_i); Serial.print(" "); Serial.println(ri2_i);
    //Serial.println(week_calculate(nian, yue, ri));

    //拼装白天和晚上的天气现象
    String text_day0, text_night0, dn0_s;
    String text_day1, text_night1, dn1_s;
    String text_day2, text_night2, dn2_s;
    /*strcmp(const char s1,const char s2)
      当 str1 < str2 时，返回为负数(-1)；
      当 str1 == str2 时，返回值= 0；
      当 str1 > str2 时，返回正数(1)。*/

    if (strcmp(future.date0_text_day, future.date0_text_night) != 0) //今天
    {
      text_day0 = future.date0_text_day;
      text_night0 = future.date0_text_night;
      dn0_s = text_day0 + "转" + text_night0;
    }
    else dn0_s = future.date0_text_night;

    if (strcmp(future.date1_text_day, future.date1_text_night) != 0) //明天
    {
      text_day1 = future.date1_text_day;
      text_night1 = future.date1_text_night;
      dn1_s = text_day1 + "转" + text_night1;
    }
    else dn1_s = future.date1_text_night;

    if (strcmp(future.date2_text_day, future.date2_text_night) != 0) //后天
    {
      text_day2 = future.date2_text_day;
      text_night2 = future.date2_text_night;
      dn2_s = text_day2 + "转" + text_night2;
    }
    else dn2_s = future.date2_text_night;

    //拼装高低温
    String  high0, high1, high2, low0, low1, low2, hl0_s, hl1_s, hl2_s;
    high0 = future.date0_high; high1 = future.date1_high; high2 = future.date2_high;
    low0 = future.date0_low; low1 = future.date1_low; low2 = future.date2_low;
    hl0_s = high0 + "/" + low0;
    hl1_s = high1 + "/" + low1;
    hl2_s = high2 + "/" + low2;

    //拼装未来天气详情并显示
    String wltqxq0 = "今 " + day0 + " " + week_calculate(nian0_i, yue0_i, ri0_i) + " " + dn0_s + " " + hl0_s;
    String wltqxq1 = "明 " + day1  + " " + week_calculate(nian1_i, yue1_i, ri1_i) + " " + dn1_s + " " + hl1_s;
    String wltqxq2 = "后 " + day2  + " " + week_calculate(nian2_i, yue2_i, ri2_i)  + " " + dn2_s + " " + hl2_s;
    //计数长度
    uint16_t wltqxq0_length = u8g2Fonts.getUTF8Width(wltqxq0.c_str());
    uint16_t wltqxq1_length = u8g2Fonts.getUTF8Width(wltqxq1.c_str());
    uint16_t wltqxq2_length = u8g2Fonts.getUTF8Width(wltqxq2.c_str());
    //自动居中的X坐标
    int wltqxq0_x, wltqxq1_x, wltqxq2_x;
    wltqxq0_x = (display.width() / 2) - (wltqxq0_length / 2);
    wltqxq1_x = (display.width() / 2) - (wltqxq1_length / 2);
    wltqxq2_x = (display.width() / 2) - (wltqxq2_length / 2);
    //拼装数据 分成2段（今 06-21）&（周一 阴转阵雨）
    String data0 = "今 " + day0; //今 06-21
    String data1 = week_calculate(nian0_i, yue0_i, ri0_i) + " " + dn0_s; //周一 阴转阵雨
    String data2 = "明 " + day1; //明 06-22
    String data3 = week_calculate(nian1_i, yue1_i, ri1_i) + " " + dn1_s; //周二 阵雨转大雨
    String data4 = "后 " + day2; //后 06-23
    String data5 = week_calculate(nian2_i, yue2_i, ri2_i) + " " + dn2_s; //周三 大雨转中雨

    uint16_t data0_length = u8g2Fonts.getUTF8Width(data0.c_str());
    uint16_t data1_length = u8g2Fonts.getUTF8Width(data1.c_str());
    uint16_t data2_length = u8g2Fonts.getUTF8Width(data2.c_str());
    uint16_t data3_length = u8g2Fonts.getUTF8Width(data3.c_str());
    uint16_t data4_length = u8g2Fonts.getUTF8Width(data4.c_str());
    uint16_t data5_length = u8g2Fonts.getUTF8Width(data5.c_str());
    //对比三个数谁大
    int data_x0_num[3] = {data0_length, data2_length, data4_length};
    int data_x0_len = sizeof(data_x0_num) / sizeof(*data_x0_num);
    int data_x0_max = get_max_num(data_x0_num, data_x0_len);
    //对比三个数谁大
    int data_x1_num[3] = {data1_length, data3_length, data5_length};
    int data_x1_len = sizeof(data_x1_num) / sizeof(*data_x1_num);
    int data_x1_max = get_max_num(data_x1_num, data_x1_len);
    //对比三个数谁小
    int wltqxq_num[3] = {wltqxq0_x, wltqxq1_x, wltqxq2_x};
    int wltqxq_len = sizeof(wltqxq_num) / sizeof(*wltqxq_num);
    int wltqxq_x_min = get_min_num(wltqxq_num, wltqxq_len) + 2;
    //数据之间的间隔 5个像素
    uint8_t jianGe_x = 5;

    u8g2Fonts.setCursor(wltqxq_x_min, day_y0);
    u8g2Fonts.print(data0);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x, day_y0);
    u8g2Fonts.print(data1);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x + data_x1_max + jianGe_x, day_y0);
    u8g2Fonts.print(hl0_s);

    u8g2Fonts.setCursor(wltqxq_x_min, day_y1);
    u8g2Fonts.print(data2);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x, day_y1);
    u8g2Fonts.print(data3);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x + data_x1_max + jianGe_x, day_y1);
    u8g2Fonts.print(hl1_s);

    u8g2Fonts.setCursor(wltqxq_x_min, day_y2);
    u8g2Fonts.print(data4);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x, day_y2);
    u8g2Fonts.print(data5);
    u8g2Fonts.setCursor(wltqxq_x_min + data_x0_max + jianGe_x + data_x1_max + jianGe_x, day_y2);
    u8g2Fonts.print(hl2_s);

    //display.drawLine(275, 0, 275, 128, 0);
    //****************************************************************************************************

    //****** 显示屏幕中间的一句话 ******
    display.drawLine(0, 56, 400, 56, 0); //画水平线
    display.drawLine(0, 74, 400, 74, 0); //画水平线
    if (eepUserSet.inAWord_mod == 0)    //显示随机句子
    {
      u8g2Fonts.setCursor(getCenterX("Hello"), 71);
      u8g2Fonts.print("Hello");
    }
    
    //************* 显示温湿度-sht30 *************
    if (dht30_temp != 0.0 && dht30_humi != 0.0)
    {
      u8g2Fonts.setFont(u8g2_font_ncenB08_te);
      display.drawInvertedBitmap(277, 76, Bitmap_tempSHT30, 16, 16, heise);
      display.drawInvertedBitmap(277, 103, Bitmap_humiditySHT30, 16, 16, heise);
      //dht30_temp = -2.5;
      //dht30_humi = 5.3;

      //以下计算用于数值和图标居中
      String zf_temp = (String)fabs(dht30_temp);  // float转换成String 取绝对值
      const char* zf_t = zf_temp.c_str();         // String转换char
      uint16_t zf_t_Width = u8g2Fonts.getUTF8Width(zf_t);

      String zf_humi = (String)dht30_humi;
      const char* zf_h = zf_humi.c_str();//String转换char
      uint16_t zf_h_Width = u8g2Fonts.getUTF8Width(zf_h);

      u8g2Fonts.setCursor(285 - zf_t_Width / 2 + 3, 101);
      u8g2Fonts.print(dht30_temp, 1);
      u8g2Fonts.setCursor(285 - zf_h_Width / 2 + 3, 128);
      u8g2Fonts.print(dht30_humi, 1);
      //display.drawLine(285, 0, 285, 128, 0);
    }

    //电量显示 及 低压提示
    //u8g2Fonts.setFont(chinese_gb2312);
    u8g2Fonts.setFont(u8g2_font_u8glib_4_tf); //u8g2_font_ncenB08_te
    if (eepUserSet.batDisplayType == 0)  //显示电压数值
    {
      String batVcc_s = String(bat_vcc) + "V";
      u8g2Fonts.setCursor(1, 113);
      u8g2Fonts.print(batVcc_s);
      //电量格数
      if (bat_vcc > 3.7)display.drawInvertedBitmap(0, 115, Bitmap_bat3, 21, 12, heise);
      else if (bat_vcc > 3.5)display.drawInvertedBitmap(0, 115, Bitmap_bat2, 21, 12, heise);
      else display.drawInvertedBitmap(6, 115, Bitmap_bat1, 21, 12, heise);
      //display.drawInvertedBitmap(2, 78, Bitmap_wifidk, 26, 22, heise); //wifi断开
      //display.drawInvertedBitmap(7, 79, Bitmap_dlsd, 19, 31, heise);  //闪电图标
    }
    else  //显示电压百分比
    {
      String batVcc_s = String(uint8_t(getBatVolBfb(bat_vcc))) + "%";
      u8g2Fonts.setCursor(4, 113);
      u8g2Fonts.print(batVcc_s);
      //电量格数
      if (bat_vcc > 3.7)display.drawInvertedBitmap(0, 115, Bitmap_bat3, 21, 12, heise);
      else if (bat_vcc > 3.5)display.drawInvertedBitmap(0, 115, Bitmap_bat2, 21, 12, heise);
      else display.drawInvertedBitmap(2, 115, Bitmap_bat1, 21, 12, heise);
      //display.drawInvertedBitmap(0, 80, Bitmap_wifidk, 26, 22, heise); //wifi断开
      //display.drawInvertedBitmap(2, 79, Bitmap_dlsd, 19, 31, heise); //闪电图标
      //Serial.print("zf_length:"); Serial.println(zf_length);
    }
    if (bat_vcc <= BatLow) //电池电压大于阈值 显示闪电图标和电量
    {
      if (dht30_temp != 0.0 && dht30_humi != 0.0) //电池电压小于阈值并 温湿度传感器存在 则在屏幕最右侧显示低压图标
      {
        display.drawInvertedBitmap(1, 92, Bitmap_batlow, 24, 35, heise);
      }
      else //电池电压小于阈值并 温湿度传感器不存在 则在屏幕最左侧显示低压图标
      {
        display.drawInvertedBitmap(270, 92, Bitmap_batlow, 24, 35, heise);
      }
      //display_bitmap_sleep("* 电量过低 *");
    }
  }
  while (display.nextPage());
}

void display_tbpd() //天气图标显示
{
  uint16_t x = circle_x + 20; //计算图标的位置
  uint16_t y = 5;
  String code;
  code = actual.weather_code;
  if (code == "0")      display.drawInvertedBitmap(x, y + 5, Bitmap_qt, 45, 45, heise);
  else if (code == "1") display.drawInvertedBitmap(x, y, Bitmap_qt_ws, 45, 45, heise);
  else if (code == "2") display.drawInvertedBitmap(x, y + 5, Bitmap_qt, 45, 45, heise);
  else if (code == "3") display.drawInvertedBitmap(x, y, Bitmap_qt_ws, 45, 45, heise);
  else if (code == "4") display.drawInvertedBitmap(x, y, Bitmap_dy, 45, 45, heise);
  else if (code == "5") display.drawInvertedBitmap(x, y, Bitmap_dy, 45, 45, heise);
  else if (code == "6") display.drawInvertedBitmap(x, y, Bitmap_dy_ws, 45, 45, heise);
  else if (code == "7") display.drawInvertedBitmap(x, y, Bitmap_dy, 45, 45, heise);
  else if (code == "8") display.drawInvertedBitmap(x, y, Bitmap_dy_ws, 45, 45, heise);
  else if (code == "9") display.drawInvertedBitmap(x, y + 3, Bitmap_yt, 45, 45, heise);
  else if (code == "10") display.drawInvertedBitmap(x, y, Bitmap_zheny, 45, 45, heise);
  else if (code == "11") display.drawInvertedBitmap(x, y, Bitmap_lzy, 45, 45, heise);
  else if (code == "12") display.drawInvertedBitmap(x, y, Bitmap_lzybbb, 45, 45, heise);
  else if (code == "13") display.drawInvertedBitmap(x, y, Bitmap_xy, 45, 45, heise);
  else if (code == "14") display.drawInvertedBitmap(x, y, Bitmap_zhongy, 45, 45, heise);
  else if (code == "15") display.drawInvertedBitmap(x, y, Bitmap_dayu, 45, 45, heise);
  else if (code == "16") display.drawInvertedBitmap(x, y, Bitmap_by, 45, 45, heise);
  else if (code == "17") display.drawInvertedBitmap(x, y, Bitmap_dby, 45, 45, heise);
  else if (code == "18") display.drawInvertedBitmap(x, y, Bitmap_tdby, 45, 45, heise);
  else if (code == "19") display.drawInvertedBitmap(x, y, Bitmap_dongy, 45, 45, heise);
  else if (code == "20") display.drawInvertedBitmap(x, y, Bitmap_yjx, 45, 45, heise);
  else if (code == "21") display.drawInvertedBitmap(x, y, Bitmap_zhenx, 45, 45, heise);
  else if (code == "22") display.drawInvertedBitmap(x, y, Bitmap_xx, 45, 45, heise);
  else if (code == "23") display.drawInvertedBitmap(x, y, Bitmap_zhongy, 45, 45, heise);
  else if (code == "24") display.drawInvertedBitmap(x, y, Bitmap_dx, 45, 45, heise);
  else if (code == "25") display.drawInvertedBitmap(x, y, Bitmap_bx, 45, 45, heise);
  else if (code == "26") display.drawInvertedBitmap(x, y, Bitmap_fc, 45, 45, heise);
  else if (code == "27") display.drawInvertedBitmap(x, y, Bitmap_ys, 45, 45, heise);
  else if (code == "28") display.drawInvertedBitmap(x, y, Bitmap_scb, 45, 45, heise);
  else if (code == "29") display.drawInvertedBitmap(x, y, Bitmap_scb, 45, 45, heise);
  else if (code == "30") display.drawInvertedBitmap(x, y + 5, Bitmap_w, 45, 45, heise);
  else if (code == "31") display.drawInvertedBitmap(x, y, Bitmap_m, 45, 45, heise);
  else if (code == "32") display.drawInvertedBitmap(x, y, Bitmap_f, 45, 45, heise);
  else if (code == "33") display.drawInvertedBitmap(x, y, Bitmap_f, 45, 45, heise);
  else if (code == "34") display.drawInvertedBitmap(x, y, Bitmap_jf, 45, 45, heise);
  else if (code == "35") display.drawInvertedBitmap(x, y, Bitmap_rdfb, 45, 45, heise);
  //else if (code == 37) display.drawInvertedBitmap(x,y, Bitmap_dy, 45, 45, heise);
  //else if (code == 38) display.drawInvertedBitmap(x,y, Bitmap_dy, 45, 45, heise);
  else if (code == "99") display.drawInvertedBitmap(x, y, Bitmap_wz, 45, 45, heise);
}
void display_tbcs() //图标测试
{
  display.fillScreen(baise);  // 填充屏幕
  display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲

  display.drawInvertedBitmap(0, 0, Bitmap_qt, 45, 45, heise);
  display.drawInvertedBitmap(50, 0, Bitmap_dy, 45, 45, heise);
  display.drawInvertedBitmap(100, 0, Bitmap_yt, 45, 45, heise);
  display.drawInvertedBitmap(150, 0, Bitmap_zheny, 45, 45, heise);
  display.drawInvertedBitmap(200, 0, Bitmap_lzybbb, 45, 45, heise);
  display.drawInvertedBitmap(250, 0, Bitmap_xy, 45, 45, heise);

  display.drawInvertedBitmap(0, 40, Bitmap_zhongy, 45, 45, heise);
  display.drawInvertedBitmap(50, 40, Bitmap_dayu, 45, 45, heise);
  display.drawInvertedBitmap(100, 40, Bitmap_by, 45, 45, heise);
  display.drawInvertedBitmap(150, 40, Bitmap_dby, 45, 45, heise);
  display.drawInvertedBitmap(200, 40, Bitmap_tdby, 45, 45, heise);
  display.drawInvertedBitmap(250, 40, Bitmap_dongy, 45, 45, heise);

  display.drawInvertedBitmap(0, 80, Bitmap_yjx, 45, 45, heise);
  display.drawInvertedBitmap(50, 80, Bitmap_zhenx, 45, 45, heise);
  display.drawInvertedBitmap(100, 80, Bitmap_xx, 45, 45, heise);
  display.drawInvertedBitmap(150, 80, Bitmap_zhongx, 45, 45, heise);
  display.drawInvertedBitmap(200, 80, Bitmap_dx, 45, 45, heise);
  display.drawInvertedBitmap(250, 80, Bitmap_bx, 45, 45, heise);
  display.display(1);
  delay(5000);

  display.fillScreen(baise);  // 填充屏幕
  display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲

  display.drawInvertedBitmap(0, 0, Bitmap_fc, 45, 45, heise);
  display.drawInvertedBitmap(50, 0, Bitmap_ys, 45, 45, heise);
  display.drawInvertedBitmap(100, 0, Bitmap_scb, 45, 45, heise);
  display.drawInvertedBitmap(150, 0, Bitmap_w, 45, 45, heise);
  display.drawInvertedBitmap(200, 0, Bitmap_m, 45, 45, heise);
  display.drawInvertedBitmap(250, 0, Bitmap_f, 45, 45, heise);

  display.drawInvertedBitmap(0, 40, Bitmap_jf, 45, 45, heise);
  display.drawInvertedBitmap(50, 40, Bitmap_rdfb, 45, 45, heise);
  display.drawInvertedBitmap(100, 40, Bitmap_ljf, 45, 45, heise);
  display.drawInvertedBitmap(150, 40, Bitmap_wz, 45, 45, heise);
  display.drawInvertedBitmap(200, 40, Bitmap_qt_ws, 45, 45, heise);
  display.drawInvertedBitmap(250, 40, Bitmap_yt_ws, 45, 45, heise);

  display.drawInvertedBitmap(0, 80, Bitmap_dy_ws, 45, 45, heise);
  display.drawInvertedBitmap(50, 80, Bitmap_zy_ws, 45, 45, heise);
  display.drawInvertedBitmap(100, 80, Bitmap_zx_ws, 45, 45, heise);

  display.display(1);
}

//****** 冒泡算法从小到大排序 ******
int get_max_num(int arr[], int len)
{
  int i, j;

  for (i = 0; i < len - 1; i++)
  {
    for (j = 0; j < len - 1 - i; j++)
    {
      if (arr[j] > arr[j + 1])
      {
        swap(&arr[j], &arr[j + 1]);
      }
    }
  }
  return arr[len - 1]; //获取最大值
}

int get_min_num(int arr[], int len)
{
  int i, j;

  for (i = 0; i < len - 1; i++)
  {
    for (j = 0; j < len - 1 - i; j++)
    {
      if (arr[j] > arr[j + 1])
      {
        swap(&arr[j], &arr[j + 1]);
      }
    }
  }
  return arr[0]; //获取最小值
}
void swap(int *t1, int *t2)
{
  int temp;
  temp = *t1;
  *t1 = *t2;
  *t2 = temp;
}
