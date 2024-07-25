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
  unsigned long currentMillis = millis();

  // 检查是否需要更新天气信息
    if (currentMillis - previousWeatherMillis >= weatherInterval) {
        updateWeather();
        previousWeatherMillis = currentMillis;
    }

  // 检查是否需要更新时间
    updateTime();
}
//获取并更新日期和天气
void updateWeather() {
  // 获取天气信息
  String weatherUrl = "http://t.weather.itboy.net/api/weather/city/101110101";
  HTTPClient http;
  http.begin(weatherUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    const String payload = http.getString();

    // 解析天气信息
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    const char *hightTemp = doc["data"]["forecast"][0]["high"];
    const char *lowTemp = doc["data"]["forecast"][0]["low"];
    const char *weather = doc["data"]["forecast"][0]["type"];
    const char *fx = doc["data"]["forecast"][0]["fx"];
    const char *fl = doc["data"]["forecast"][0]["fl"];
    const char *date = doc["date"];
    // 解析年份、月份和日期
   
    // 分割日期字符串
    strncpy(riqi.year, date, 4);
    riqi.year[4] = '\0';  // 添加字符串结束符

    strncpy(riqi.month, date + 4, 2);
    riqi.month[2] = '\0';  // 添加字符串结束符

    strncpy(riqi.day, date + 6, 2);
    riqi.day[2] = '\0';  // 添加字符串结束符
    String currentDate = "日期:" + String(riqi.year) + "-" + String(riqi.month) + "-" + String(riqi.day);
    display_partialLine(3, currentDate);

    String tq = "天气:" + String(weather);
    display_partialLine(4, tq);

    String zgwd = "最高温度:" + String(hightTemp);
    display_partialLine(13, zgwd);

    String zdwd = "最低温度:" + String(lowTemp);
    display_partialLine(14, zdwd);

    String f_x = "风向:" + String(fx);
    display_partialLine(15, f_x);

    String f_l = "风力:" + String(fl);
    display_partialLine(16, f_l);

    syncTime();
  }

  http.end();
}
