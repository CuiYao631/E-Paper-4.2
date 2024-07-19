

//计算今天是周几的公式  d为几号，m为月份，y为年份
String week_calculate(int y, int m, int d)
{
  if (m == 1 || m == 2)  //一二月换算
  {
    m += 12;
    y--;
  }
  int week = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;
  if (week == 1) return"周一";
  else if (week == 2 ) return"周二";
  else if (week == 3 ) return"周三";
  else if (week == 4 ) return"周四";
  else if (week == 5 ) return"周五";
  else if (week == 6 ) return"周六";
  else if (week == 0 ) return"周日";
  else return "计算出错";
  //其中1~6表示周一到周六 0表示星期天
}
uint16_t getCenterX(String z) //计算字符居中的X位置
{
  uint16_t zf_width = u8g2Fonts.getUTF8Width(z.c_str());  //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);    //计算字符居中的X位置
  return x;
}
uint16_t getCenterX(const char* z) //计算字符居中的X位置
{
  uint16_t zf_width = u8g2Fonts.getUTF8Width(z);        //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);  //计算字符居中的X位置
  return x;
}
double getBatVolBfb(double batVcc) //获取电压的百分比，经过换算并非线性关系
{
  double bfb = 0.0;
  //y = 497.50976 x4 - 7,442.07254 x3 + 41,515.70648 x2 - 102,249.34377 x + 93,770.99821
  bfb = 497.50976 * batVcc * batVcc * batVcc * batVcc
        - 7442.07254 * batVcc * batVcc * batVcc
        + 41515.70648 * batVcc * batVcc
        - 102249.34377 * batVcc
        + 93770.99821;
  if (bfb > 100) bfb = 100.0;
  else if (bfb < 0) bfb = 3.0;
  return bfb;
}
