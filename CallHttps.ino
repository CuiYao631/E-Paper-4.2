
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
}
