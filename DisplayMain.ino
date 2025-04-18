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
    if (currentMillis - previousSyncMillis >= syncInterval && !needSync) {
        // 在主循环中已经处理同步，这里不需要重复操作
    }

  // 检查是否需要更新时间
    // 时间更新已在主循环中处理
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

    // const int forecastDays = 5; // 需要保存的预测天数
    // const char* highTemp[forecastDays];
    // const char* lowTemp[forecastDays];
    // const char* weather[forecastDays];
    // const char* fx[forecastDays];
    // const char* fl[forecastDays];
    // const char* week[forecastDays];

    for (int i = 0; i < forecastDays; i++) {
      future.highTemp[i] = doc["data"]["forecast"][i]["high"];
      future.lowTemp[i] = doc["data"]["forecast"][i]["low"];
      future.weather[i] = doc["data"]["forecast"][i]["type"];
      future.fx[i] = doc["data"]["forecast"][i]["fx"];
      future.fl[i] = doc["data"]["forecast"][i]["fl"];
      future.week[i]=doc["data"]["forecast"][i]["week"];
      future.notice[i]=doc["data"]["forecast"][i]["notice"];
    }

    const char *date = doc["date"];
    
    // 分割日期字符串
    strncpy(riqi.year, date, 4);
    riqi.year[4] = '\0';  // 添加字符串结束符

    strncpy(riqi.month, date + 4, 2);
    riqi.month[2] = '\0';  // 添加字符串结束符

    strncpy(riqi.day, date + 6, 2);
    riqi.day[2] = '\0';  // 添加字符串结束符
    
    //display_partialLine(3, currentDate);

    
    drawTitle();
    drawMiddle();
    drawBottom();

    syncTime();
  }

  http.end();
}
