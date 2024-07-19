/**
 * @author: XCuiTech
 * @description: 各种参数配置
 */
#define baise  GxEPD_WHITE  //白色
#define heise  GxEPD_BLACK  //黑色

//GPIO
#define PIN_CS 33
#define PIN_CS1 32
#define PIN_DC 26
#define PIN_RST 25
#define PIN_BUSY 14
#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_LED 4
#define PIN_BTN_1 0
#define PIN_BTN_2 2
#define PIN_BTN_3 15
#define PIN_BTN_4 13

//固定参数
#define DURATION 3000 // 长按时间

int dht30_error = 0;            // dht30错误代码
float dht30_temp = 0.0;         // dht30温度
float dht30_humi = 0.0;         // dht30湿度
float bat_vcc = 0.0;                // 电池电压
#define BatLow          3.3         // 低压提示和永不唤醒电压

uint32_t RTC_minute = 100;       // RTC数据-分钟
uint32_t RTC_hour = 100;         // RTC数据-小时
uint32_t RTC_seconds = 100;      // RTC数据-秒
uint32_t RTC_cq_cuont = 0;       // RTC数据-重启计数
uint32_t RTC_clockTime2 = 0;     // RTC数据-上次的时间
uint32_t RTC_wifi_ljcs = 0;      // RTC数据-wifi连接超时计数
uint32_t RTC_jsjs = 0;           // RTC数据-局刷计数
uint32_t RTC_ntpTimeError = 0;   // 获取时间超时 0-无 1-超时 2-wifi连接超时
uint32_t RTC_clock_code = 0;     // 时钟错误代码
uint32_t RTC_Bfen_code = 0;      // B粉错误代码
uint32_t RTC_follower = 0;       // B粉数量 //http://api.bilibili.com/x/relation/stat?vmid=16758526&jsonp=jsonp
uint32_t RTC_8025T_error = 0;    // BL8025T时钟芯片错误检测 0-正常 1-年错 2-月错 3-日错 4-时错 5-分错 6-秒错
uint32_t RTC_albumAutoOldWz = 0; // 随机轮播图片的上一次位置
uint32_t RTC_tqmskjxs = 1;       // 天气模式开机显示 0-不显示 其他-显示
uint32_t RTC_SDInitError = 0;    // SD挂载错误 0-无 1-错误
uint32_t clockTime0, clockTime1;
uint32_t clockTime3 = 0;
