/**
 * @file    config.h
 * @author  XCuiTech
 * @date    2025年4月21日
 * @brief   各种参数配置
 */

#ifndef CONFIG_H
#define CONFIG_H

/*----------------------------------------- 显示屏配置 -----------------------------------------*/
#define baise GxEPD_WHITE // 白色
#define heise GxEPD_BLACK // 黑色

/*----------------------------------------- GPIO引脚定义 --------------------------------------*/
/* 屏幕引脚 */
#define PIN_CS   27      // 屏幕 CS引脚
#define PIN_CS1  26      // 屏幕 CS1引脚
#define PIN_DC   5       // 屏幕 DC引脚
#define PIN_RST  17      // 屏幕 RST引脚
#define PIN_BUSY 16     // 屏幕 BUSY引脚

/* SD卡引脚 */
#define SD_CS_PIN   33  // SD卡 CS引脚
#define SD_MISO_PIN 13  // SD卡 MISO引脚
#define SD_MOSI_PIN 12  // SD卡 MOSI引脚
#define SD_CLK_PIN  14  // SD卡 CLK引脚

/* I2C引脚 */
#define PIN_SDA 21      // I2C引脚-SDA
#define PIN_SCL 22      // I2C引脚-SCL

/* LED和按钮引脚 */
#define PIN_LED  4      // RGB_LED引脚
#define BTN_PIN1 32     // 按钮 1引脚
#define BTN_PIN2 35     // 按钮 2引脚
#define BTN_PIN3 34     // 按钮 3引脚
#define BTN_PIN4 25     // 按钮 4引脚

/*----------------------------------------- 按键配置 -----------------------------------------*/
#define DURATION 3000   // 长按时间（毫秒）

/*----------------------------------------- 网络配置 -----------------------------------------*/
/* NTP配置 */
#define NTPAdress       "ntp.aliyun.com"   // NTP服务器地址
#define TIME_INTERVAL   60000              // 更新时间间隔(单位:毫秒)
#define TIME_ZONE_OFFSET 8 * 3600          // 东八区时间偏移量(单位:秒)

/* 天气API配置 */
#define WEATHER_API     "http://t.weather.itboy.net/api/weather/city/101110101" // 天气API地址

/*----------------------------------------- 传感器数据 -----------------------------------------*/
/* DHT30传感器数据 */
int dht30_error = 0;    // dht30错误代码
float dht30_temp = 0.0; // dht30温度
float dht30_humi = 0.0; // dht30湿度

/* 电池相关 */
float bat_vcc = 0.0;    // 电池电压
#define BatLow 3.3      // 低压提示和永不唤醒电压

/*----------------------------------------- RTC时间配置 -----------------------------------------*/
/* RTC数据 */
uint32_t RTC_minute = 100;       // RTC数据-分钟
uint32_t RTC_hour = 100;         // RTC数据-小时
uint32_t RTC_seconds = 100;      // RTC数据-秒
uint32_t RTC_cq_cuont = 0;       // RTC数据-重启计数

/* 时间戳记录 */
unsigned long previousWeatherMillis = 0; // 上次更新天气的时间
unsigned long previousTimeMillis = 0;    // 上次更新时间的时间
unsigned long previousSyncMillis = 0;    // 上次联网同步的时间(天气和NTP)

/*----------------------------------------- 系统参数配置 -----------------------------------------*/
/* 时间间隔 */
const long syncInterval = 3600000;       // 1小时的毫秒数(联网同步天气和时间)
const long timeInterval = 1000;          // 1秒的毫秒数(更新时间)

/* WiFi配置 */
const int maxRetries = 20;               // 最大WiFi重试次数
const int wifiRetryDelay = 500;          // WiFi重试延迟(毫秒)

/* 睡眠配置 */
const int deepSleepInterval = 60;        // 深度睡眠时间(秒)

#endif /* CONFIG_H */
