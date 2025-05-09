/**
 * @file    config.h
 * @author  XCuiTech
 * @date    2025年5月8日
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
#define PIN_BUSY 16      // 屏幕 BUSY引脚

/* SD卡引脚 */
#define SD_CS_PIN   33   // SD卡 CS引脚
#define SD_MISO_PIN 13   // SD卡 MISO引脚
#define SD_MOSI_PIN 12   // SD卡 MOSI引脚
#define SD_CLK_PIN  14   // SD卡 CLK引脚

/* I2C引脚 */
#define PIN_SDA 21       // I2C引脚-SDA
#define PIN_SCL 22       // I2C引脚-SCL

/* LED和按钮引脚 */
#define PIN_LED  4       // RGB_LED引脚
#define BTN_PIN1 32      // 按钮 1引脚
#define BTN_PIN2 35      // 按钮 2引脚
#define BTN_PIN3 34      // 按钮 3引脚
#define BTN_PIN4 25      // 按钮 4引脚

/* 电池检测引脚 */
#define BATTERY_PIN 39   // 电池电压读取引脚

/*----------------------------------------- 按键配置 -----------------------------------------*/
#define DURATION 3000    // 长按时间（毫秒）

/*----------------------------------------- 网络配置 -----------------------------------------*/
/* WiFi配置 */
#define AP_NAME "E-Paper-Config"  // 热点名称
#define AP_PASSWORD "12345678"    // 热点密码
#define AP_TIMEOUT 180            // 配置门户超时时间（秒）
#define MAX_RETRIES 20            // 最大WiFi重试次数
#define WIFI_RETRY_DELAY 500      // WiFi重试延迟(毫秒)

/* NTP配置 */
#define NTPAdress       "ntp.aliyun.com"   // NTP服务器地址
#define TIME_INTERVAL   60000              // 更新时间间隔(单位:毫秒)
#define TIME_ZONE_OFFSET 8 * 3600          // 东八区时间偏移量(单位:秒)

/* 天气API配置 */
#define WEATHER_API     "http://t.weather.itboy.net/api/weather/city/101110101" // 天气API地址

/*----------------------------------------- 传感器配置 -----------------------------------------*/
/* 传感器读取间隔 */
#define SENSOR_READ_INTERVAL 60000  // 传感器读取间隔(1分钟)

/* 电池参数 */
#define BATTERY_READ_INTERVAL 60000 // 电池电量读取间隔（1分钟）
#define BATTERY_MAX_VOLTAGE 4.2     // 电池最高电压（满电）
#define BATTERY_MIN_VOLTAGE 3.3     // 电池最低电压（需要充电）
#define BATTERY_SAMPLES 20          // 采样次数，提高测量精度
#define REFERENCE_VOLTAGE 3.3 // ESP32 ADC参考电压
#define VOLTAGE_DIVIDER_RATIO 2.0   // 分压电阻比例
#define BatLow 3.3                  // 低压提示和永不唤醒电压

/* DHT30传感器数据 */
extern int dht30_error;             // dht30错误代码
extern float dht30_temp;            // dht30温度
extern float dht30_humi;            // dht30湿度

/* 电池相关 */
extern float bat_vcc;               // 电池电压

/*----------------------------------------- 界面布局配置 -----------------------------------------*/
/* 进度条参数 */
#define PROGRESS_BAR_Y 200          // 进度条Y坐标
#define PROGRESS_BAR_WIDTH 200      // 进度条宽度
#define PROGRESS_BAR_HEIGHT 20      // 进度条高度

/* 传感器数据显示位置 */
#define TEMP_X 1                    // 温度位置X
#define TEMP_Y 135                  // 温度位置Y
#define HUMI_X 340                    // 湿度位置X
#define HUMI_Y 135                    // 湿度位置Y

/* 时间显示位置 */
#define TIME_X 66                   // 时间位置X
#define TIME_Y 140                  // 时间位置Y

/* 天气信息布局 */
#define DAY_Y0 90                   // 未来天气详情Y坐标
#define DAY_Y1 (DAY_Y0+18)          // 第二行Y坐标
#define DAY_Y2 (DAY_Y1+18)          // 第三行Y坐标

/* 图标相关位置 */
#define MAIN_Y0 14                  // 更新时间图标Y坐标
#define MAIN_Y1 (MAIN_Y0+19)        // 位置图标Y坐标
#define MAIN_Y2 (MAIN_Y1+19)        // 天气状态图标Y坐标
#define MAIN_X0 1                   // 左边小图标X位置
#define MAIN_X1 (MAIN_X0+15)        // 左边小图标后的文字的X位置
#define MAIN_X2 (400-12)            // 右边小图标X位置
#define TEMP_Y0 50                  // 温度Y位置

/*----------------------------------------- RTC时间配置 -----------------------------------------*/
/* RTC数据 */
extern uint32_t RTC_minute;         // RTC数据-分钟
extern uint32_t RTC_hour;           // RTC数据-小时
extern uint32_t RTC_seconds;        // RTC数据-秒
extern uint32_t RTC_cq_cuont;       // RTC数据-重启计数

/* 时间戳记录 */
extern unsigned long previousWeatherMillis; // 上次更新天气的时间
extern unsigned long previousTimeMillis;    // 上次更新时间的时间
extern unsigned long previousSyncMillis;    // 上次联网同步的时间(天气和NTP)

/*----------------------------------------- 系统参数配置 -----------------------------------------*/
/* 时间间隔 */
#define SYNC_INTERVAL 3600000       // 1小时的毫秒数(联网同步天气和时间)
#define TIME_UPDATE_INTERVAL 1000   // 1秒的毫秒数(更新时间)

/* 睡眠配置 */
#define DEEP_SLEEP_INTERVAL 60      // 深度睡眠时间(秒)

#endif /* CONFIG_H */
