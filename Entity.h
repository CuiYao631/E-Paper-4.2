

//****** EEPROM地址和定义 ******
#define eeprom_address0 0    //起始地址
struct EEPROMStruct
{
  uint8_t auto_state;        // 自动刷写eeprom状态 0-需要 1-不需要
  char  city[30];            // 城市
  char  weatherKey[24];      // 天气KEY
  boolean nightUpdata;       // 夜间更新 1-更新 0-不更新
  char  inAWord[67];         // 自定义一句话
  uint8_t inAWord_mod;       // 自定义一句话的模式 0-联网获取句子 1-自定义句子 2-天数倒计时 3-B站粉丝
  boolean batDisplayType;    // 电量显示类型 0-电压 1-百分比
  uint8_t runMode;           // 0-模式选择页面 1-天气 2-阅读 3-时钟 4-配网
  char txtNameLastTime[32];  // 上一次打开的txt文件名
  char customBmp[32];        // 要显示的自定义图片
  int16_t clockCompensate;   // 时钟手动补偿值
  float outputPower;         // 设置发射功率
  boolean setRotation;       // 屏幕旋转方向0-90度（1） 1-270度（3）
  boolean clock_display_state;     // 时钟模式是否开启日期显示
  boolean clock_calibration_state; // 时钟模式是否开启强制校准
  uint8_t clockJZJG;               // 时钟模式校准间隔 分钟
  boolean albumAuto;               // 相册自动动播放 0-关闭 1-开启
  boolean fastFlip;                // 快速翻页 0-关闭 1-开启
  uint8_t clockQSJG;               // 时钟模式全局刷新间隔
  boolean sdState;                 // 启用SD卡 1-启用 0-未启用
  boolean type_8025T;              // 8025t 的类型 0-BL(按顺序读取) 1-RX(读取的数据会偏移8位)
} eepUserSet;


//****** 天气数据
struct ActualWeather
{
  char status_code[64];  // 错误代码
  char city[16];         // 城市名称
  char weather_name[16]; // 天气现象名称
  char weather_code[4];  // 天气现象代码
  char temp[5];          // 温度
  char last_update[25];  // 最后更新时间
};
ActualWeather actual;   // 创建结构体变量 目前的天气

const int forecastDays = 5; // 需要保存的预测天数
struct FutureWeather
{
    
    const char* highTemp[forecastDays];
    const char* lowTemp[forecastDays];
    const char* weather[forecastDays];
    const char* fx[forecastDays];
    const char* fl[forecastDays];
    const char* week[forecastDays];
    const char* notice[forecastDays];
};
FutureWeather future; //创建结构体变量 未来天气
struct RiQi       // 日期 https://api.xygeng.cn/day
{
  int code;       // 错误代码
  char year[5];   // 年
  char month[3];  // 月
  char date[3];   // 日
  char day[12];   // 星期几
  char festival[64]; // 节日名
  int hours;   //小时
  int minutes; //分钟
  int seconds;  //秒
}; RiQi riqi; // 创建结构体变量 未来


struct LifeIndex // 生活指数
{
  char status_code[64];  // 错误代码
  char uvi[10];          // 紫外线指数
}; LifeIndex life_index; // 创建结构体变量 生活指数
struct Hitokoto  // 一言API
{
  char status_code[64]; //错误代码
  char hitokoto[70];
  //https://v1.hitokoto.cn?encode=json&charset=utf-8&min_length=1&max_length=21
  //https://pa-1251215871.cos-website.ap-chengdu.myqcloud.com/
};
Hitokoto yiyan; //创建结构体变量 一言

struct eepClock {
  char year[5];       // 年
  char month[3];      // 月
  char day[3];        // 日
  /*uint8_t hour;       // 时
    uint8_t minute;     // 分
    uint8_t seconds;    // 秒*/
  char week[12];      // 星期几
  char festival[64];  // 节日名
} eepUserClock;
