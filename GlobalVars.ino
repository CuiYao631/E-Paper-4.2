/**
 * @file GlobalVars.ino
 * @brief 全局变量定义文件
 * @author xiaocui
 * @date 2025年5月8日
 * 
 * 此文件负责定义在config.h中声明为extern的全局变量
 * 集中管理全局状态，提高代码可维护性
 */

#include "config.h"

/*----------------------------------------- RTC时间变量 -----------------------------------------*/
// RTC时间数据
uint32_t RTC_minute = 0;         // RTC数据-分钟
uint32_t RTC_hour = 0;           // RTC数据-小时
uint32_t RTC_seconds = 0;        // RTC数据-秒
uint32_t RTC_cq_cuont = 0;       // RTC数据-重启计数

/*----------------------------------------- 时间戳记录 -----------------------------------------*/
// 各种操作的时间戳记录
unsigned long previousWeatherMillis = 0; // 上次更新天气的时间戳
unsigned long previousTimeMillis = 0;    // 上次更新时间的时间戳
unsigned long previousSyncMillis = 0;    // 上次联网同步的时间戳(天气和NTP)

/*----------------------------------------- 时间间隔常量 -----------------------------------------*/
// 定义间隔常量
const long timeInterval = TIME_INTERVAL;          // 时间更新间隔，使用config.h中的值
const long syncInterval = SYNC_INTERVAL;          // 同步更新间隔，使用config.h中的值

/*----------------------------------------- 传感器数据变量 -----------------------------------------*/
// DHT30传感器数据
int dht30_error = 0;             // dht30错误代码
float dht30_temp = 0.0;          // dht30温度
float dht30_humi = 0.0;          // dht30湿度

// 电池相关
float bat_vcc = 0.0;             // 电池电压
