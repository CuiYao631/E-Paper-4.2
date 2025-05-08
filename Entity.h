

/**
 * Entity.h
 * 包含项目中使用的主要数据结构定义
 * 已优化移除未使用字段，提高可读性和内存利用率
 */

//****** EEPROM地址和定义 ******
#define eeprom_address0 0    //起始地址
struct EEPROMStruct
{
  char  city[30];            // 城市
  char  weatherKey[24];      // 天气KEY
  uint8_t runMode;           // 0-模式选择页面 1-天气 2-阅读 3-时钟 4-配网
  boolean setRotation;       // 屏幕旋转方向0-90度（1） 1-270度（3）
} eepUserSet;


//****** 天气数据
struct ActualWeather
{
  char city[16];         // 城市名称
  char weather_name[16]; // 天气现象名称
  char temp[5];          // 温度
};
ActualWeather actual;   // 创建结构体变量 目前的天气

const int forecastDays = 5; // 需要保存的预测天数

/**
 * 未来天气预报结构体
 * 存储未来几天的天气预报信息
 */
struct FutureWeather
{
  const char* highTemp[forecastDays]; // 最高温度
  const char* lowTemp[forecastDays];  // 最低温度
  const char* weather[forecastDays];  // 天气状况
  const char* fx[forecastDays];       // 风向
  const char* fl[forecastDays];       // 风力
  const char* week[forecastDays];     // 星期
  const char* notice[forecastDays];   // 提示信息
};
FutureWeather future; // 未来天气预报实例

/**
 * 日期时间结构体
 * 存储和管理当前日期和时间信息
 */
struct RiQi
{
  int code;         // 错误代码
  char year[5];     // 年
  char month[3];    // 月
  char day[3];      // 日
  char date[3];     // 日期
  char weekday[12]; // 星期几
  char festival[64]; // 节日名
  int hours;        // 小时
  int minutes;      // 分钟
  int seconds;      // 秒
};
RiQi riqi; // 当前日期时间实例

