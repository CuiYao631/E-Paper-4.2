
//http请求
String callHttp(String url)
{
  String payload;
  //Serial.print("requesting URL: ");
  //Serial.println(url);
  WiFiClient client;
  HTTPClient http;
  http.setReuse(2);
  if (http.begin(client, url))
  {
    delay(1);
    int httpCode = http.GET();
    delay(1);
    if (httpCode > 0)  //判断有无返回值
    {
      if (httpCode == 200 || httpCode == 304 || httpCode == 403 || httpCode == 404 || httpCode == 500) //判断请求是正确
      {
        payload = http.getString();
        //Serial.println(" ");
        //Serial.println("http.GET");
        //Serial.println(payload);
        //Serial.println(" ");
        return payload;
      }
      else
      {
        Serial.print("请求错误："); Serial.println(httpCode); Serial.println(" ");
        payload = "{\"status_code\":\"" + String("请求错误:") + String(httpCode) + "\"}";  //将错误值转换成json格式
        return payload;
      }
    }
    else
    {
      Serial.println(" "); Serial.print("GET请求错误："); Serial.println(httpCode);
      Serial.printf("[HTTP] GET... 失败, 错误: %s\n", http.errorToString(httpCode).c_str());
      payload = "{\"status_code\":\"" + String(http.errorToString(httpCode).c_str()) + "\"}";  //将错误值转换成json格式
      //Serial.println(payload);
      return payload;
    }
    http.end();
  }
  else
  {
    Serial.printf("[HTTP] 无法连接服务器\n");
    payload = "{\"status_code\":\"" + String("无法连接服务器") + "\"}";  //将错误值转换成json格式
    return payload;
  }
}


//****** 获取日期数据
boolean ParseRiQi(String content, struct RiQi* jgt)
{
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, content);
  //serializeJson(doc, Serial); // 构造序列化json,将内容从串口输出
  if (error)  // 检查API是否有返回错误信息，有返回则进入休眠或提示
  {
    Serial.print("日期数据json配置失败：");
    Serial.println(error.f_str());
    Serial.println(" ");
    String z = "日期数据json配置失败：" + String(error.f_str());
    return false;
  }

  int code = doc["code"]; // 200
  if (code == 200) {
    JsonObject data = doc["data"];
    JsonObject data_solar = data["solar"];

    if (data_solar["year"].isNull() == 0)
      strcpy(jgt->year, data_solar["year"]);
    if (data_solar["month"].isNull() == 0)
      strcpy(jgt->month, data_solar["month"]);
    if (data_solar["date"].isNull() == 0)
      strcpy(jgt->date, data_solar["date"]);
    if (data_solar["day"].isNull() == 0)
      strcpy(jgt->day, data_solar["day"]);
    if ( data["festival"][0].isNull() == 0)
      strcpy(jgt->festival,  data["festival"][0]);
    return true;
  }
  else {
    //String z = "日期数据获取失败：" + String(code);
    //const char *character = z.c_str();
    return false;
  }
  return true;
  /*Serial.println(" ");
    Serial.print("日期年："); Serial.println(riqi.year);
    Serial.print("日期月："); Serial.println(riqi.month);
    Serial.print("日期日："); Serial.println(riqi.date);
    Serial.print("日期星期："); Serial.println(riqi.day);*/
  /* const char* data_solar_year = data_solar["year"];    // "2021"
    const char* data_solar_month = data_solar["month"];  // "06"
    const char* data_solar_date = data_solar["date"];    // "19"
    const char* data_solar_day = data_solar["day"];      // "星期六"*/
  /*JsonObject data_lunar = data["lunar"];
    const char* data_lunar_year = data_lunar["year"];     // "辛丑"
    const char* data_lunar_animal = data_lunar["animal"]; // "牛"
    const char* data_lunar_month = data_lunar["month"];   // "五月"
    const char* data_lunar_date = data_lunar["date"];     // "初十 "*/
  // 复制我们感兴趣的字符串 ,先检查是否为空，空会导致系统无限重启
  // 这不是强制复制，你可以使用指针，因为他们是指向"内容"缓冲区内
  // 所以你需要确保 当你读取字符串时它仍在内存中
  // isNull()检查是否为空 空返回1 非空0
}
