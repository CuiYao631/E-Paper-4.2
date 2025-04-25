/**
 * @file    BluetoothManager.ino
 * @author  XCuiTech
 * @date    2025年4月23日
 * @brief   ESP32蓝牙管理模块 (重新设计)
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>
#include "config.h"
#include <BLE2902.h>

// 外部WiFi连接函数引用
extern bool connectToWiFi(const String& ssid, const String& password);

// BLE相关定义
BLEServer *pServer = nullptr;
BLECharacteristic *pTxCharacteristic = nullptr;
bool deviceConnected = false;
std::string completeData = "";  // 用于存储接收到的完整数据

// BLE服务和特征UUID
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART服务UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // RX特征UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // TX特征UUID

// 屏幕尺寸常量
#define SCREEN_SIZE "4.2" // 屏幕尺寸

// 蓝牙设备名称
String btDeviceName = "E-Paper";

// BLE广播配置
bool bleAdvertising = false;  // 广播状态
unsigned long lastAdvertisingUpdate = 0;  // 上次广播更新时间
const unsigned long ADVERTISING_UPDATE_INTERVAL = 30000;  // 广播数据更新间隔(毫秒)

// 蓝牙消息缓冲区
String btMessage = "";
const int MAX_BT_BUFFER = 256;

/**
 * 蓝牙连接/断开处理回调类
 */
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("蓝牙客户端已连接");
    deviceConnected = true;
    
    // 停止蓝牙广播以节省电力
    if (bleAdvertising) {
      BLEAdvertising *pAdvertising = pServer->getAdvertising();
      if (pAdvertising) {
        pAdvertising->stop();
        bleAdvertising = false;
        Serial.println("客户端连接后，BLE广播已停止");
      }
    }
  };

  void onDisconnect(BLEServer* pServer) {
    Serial.println("蓝牙客户端已断开");
    deviceConnected = false;
    
    // 给蓝牙堆栈一些时间清理
    delay(500);
    
    // 重新开始广播，使设备再次可被发现
    if (pServer) {
      Serial.println("正在重置BLE广播...");
      updateBLEAdvertisingData(); // 更新广播数据
      pServer->startAdvertising(); // 重新开始广播
      Serial.println("BLE广播已重新启动");
    }
  }
};

/**
 * 蓝牙接收数据处理回调类
 */
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    
    if (rxValue.empty()) {
      Serial.println("接收到空数据");
      return;
    }
    
    // 将分块数据追加到完整数据字符串中
    completeData += rxValue;
    Serial.println("接收数据块: " + String(rxValue.c_str()));
    
    // 检测是否数据已完整（例如：包含结束标记）
    // 此处可以根据实际通信协议调整结束标记判断
    bool isComplete = false;
    
    // 例如：检查是否包含换行符作为结束标记
    if (rxValue.find('\n') != std::string::npos || rxValue.find('\r') != std::string::npos) {
      isComplete = true;
    }
    
    if (isComplete) {
      Serial.println("接收到完整数据: " + String(completeData.c_str()));
      
      // 处理接收到的命令
      String command = String(completeData.c_str());
      command.trim(); // 去除首尾空白字符
      processBluetoothCommand(command);
      
      // 清空数据缓冲区
      completeData = "";
      
      // 发送响应数据（如果需要）
      if (deviceConnected && pTxCharacteristic != nullptr) {
        String response = "OK";
        pTxCharacteristic->setValue(response.c_str());
        pTxCharacteristic->notify();
        Serial.println("已发送响应: " + response);
      }
    }
  }
};

/**
 * @brief 更新BLE广播数据
 */
void updateBLEAdvertisingData() {
  if (!pServer) return;
  
  // 获取MAC地址
  String macAddress = WiFi.macAddress();
  
  // 准备数据字符串，用于记录日志
  String advInfo = btDeviceName + "," + macAddress + "," + String(bat_vcc) + "V," + String(SCREEN_SIZE) + "\"";
  Serial.println("更新BLE广播数据: " + advInfo);
  
  // 准备制造商数据（十六进制格式）
  // 公司ID使用0xFFFF表示测试使用
  uint16_t companyId = 0xFFFF;
  
  // 创建广播数据数组
  uint8_t manufacturerData[30] = {0};
  // 前2个字节是公司ID (Little Endian)
  manufacturerData[0] = companyId & 0xFF;
  manufacturerData[1] = (companyId >> 8) & 0xFF;
  
  // 第3个字节开始是自定义数据
  // 电池电压 - 转换为0-255范围的值
  uint8_t batLevel = (uint8_t)(min(max(bat_vcc - 2.5, 0.0), 1.5) * 170); // 将2.5V-4V映射到0-255
  manufacturerData[2] = batLevel;
  
  // 屏幕尺寸 - 转换为整数值 * 10，例如4.2 -> 42
  uint8_t screenSize = (uint8_t)(atof(SCREEN_SIZE) * 10);
  manufacturerData[3] = screenSize;
  
  // 设备类型标识 - E-Paper设备用0xEP表示
  manufacturerData[4] = 0xEE; // 'E'的ASCII的十六进制表示就是0x45，这里用0xEE更明显
  manufacturerData[5] = 0x50; // 'P'的ASCII的十六进制表示
  
  // 加入MAC地址（6个字节）
  uint8_t mac[6];
  WiFi.macAddress(mac);
  for (int i = 0; i < 6; i++) {
    manufacturerData[6 + i] = mac[i];
  }
  
  // 设置广播数据
  BLEAdvertisementData advertisementData;
  // 将二进制数据转换为Arduino String类型
  std::string manufacturerDataStr = "";
  for (int i = 0; i < 12; i++) {  // 现在使用12个字节：2字节厂商ID + 4字节数据 + 6字节MAC地址
    manufacturerDataStr += (char)manufacturerData[i];
  }
  
  advertisementData.setManufacturerData(manufacturerDataStr);
  advertisementData.setName(btDeviceName.c_str());
  
  // 获取并应用广播数据
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAdvertisementData(advertisementData);
  
  // 记录更新时间
  lastAdvertisingUpdate = millis();
}

/**
 * @brief 初始化蓝牙
 */
void initBluetooth() {
  Serial.println("初始化BLE...");

  // 初始化BLE设备
  BLEDevice::init(btDeviceName.c_str());
  
  // 创建BLE服务器
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // 创建BLE服务
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  // 创建BLE特征 - TX特征（用于向客户端发送数据）
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());
  
  // 创建BLE特征 - RX特征（用于从客户端接收数据）
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_RX,
                        BLECharacteristic::PROPERTY_WRITE | 
                        BLECharacteristic::PROPERTY_WRITE_NR);
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  
  // 启动服务
  pService->start();
  
  // 获取广播对象并启动广播
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  updateBLEAdvertisingData();
  pAdvertising->start();
  bleAdvertising = true;
  
  Serial.println("蓝牙已初始化，设备名称：" + btDeviceName);
  Serial.println("BLE服务和特征已创建");
  Serial.println("BLE广播已启动，等待客户端连接...");
}

/**
 * @brief 发送数据到蓝牙客户端
 * @param data 要发送的数据
 */
void sendBluetoothData(String data) {
  if (deviceConnected && pTxCharacteristic != nullptr) {
    // 添加结束符（换行符）到数据末尾
    data += '\n';
    pTxCharacteristic->setValue(data.c_str());
    pTxCharacteristic->notify();
    Serial.println("蓝牙发送: " + data);
  }
}

/**
 * @brief 解析JSON格式的命令字符串
 * @param jsonCmd JSON格式的命令字符串
 */
void processJsonCommand(String jsonCmd) {
  Serial.println("处理JSON命令: " + jsonCmd);
  
  // 提取cmd字段值
  int cmdStart = jsonCmd.indexOf("\"cmd\":\"") + 7;
  int cmdEnd = jsonCmd.indexOf("\"", cmdStart);
  
  if (cmdStart > 7 && cmdEnd > cmdStart) {
    String cmdValue = jsonCmd.substring(cmdStart, cmdEnd);
    Serial.println("提取的命令: " + cmdValue);
    
    // 提取seq字段值
    int seqStart = jsonCmd.indexOf("\"seq\":") + 6;
    int seqEnd = jsonCmd.indexOf(",", seqStart);
    int seq = 0;
    if (seqStart > 6 && seqEnd > seqStart) {
      seq = jsonCmd.substring(seqStart, seqEnd).toInt();
    }
    
    // 根据命令类型执行相应操作
    if (cmdValue == "get_info") {
      // 获取设备详细信息
      String mac = WiFi.macAddress();
      String ip = WiFi.localIP().toString();
      String wifiStatus = WiFi.status() == WL_CONNECTED ? "connected" : "disconnected";
      
      // 生成设备ID（从MAC地址创建）
      String deviceId = "ESP32-";
      // 取MAC地址的后半部分并去掉冒号
      for (int i = 9; i < mac.length(); i++) {
        if (mac.charAt(i) != ':') {
          deviceId += mac.charAt(i);
        }
      }
      
      // 创建JSON响应
      String response = "{";
      response += "\"cmd\":\"get_info\",";
      response += "\"seq\":" + String(seq) + ",";
      response += "\"data\":{";
      response += "\"device_id\":\"" + deviceId + "\",";
      response += "\"firmware_version\":\"1.0.0\",";
      response += "\"mac\":\"" + mac + "\",";
      response += "\"wifi_status\":\"" + wifiStatus + "\",";
      response += "\"ip\":\"" + ip + "\"";
      response += "}";
      response += "}";
      
      sendBluetoothData(response);
    }
    else if (cmdValue == "set_wifi") {
      // 解析WiFi设置命令，提取ssid和password
      int dataStart = jsonCmd.indexOf("\"data\":{");
      if (dataStart > 0) {
        // 提取SSID
        int ssidStart = jsonCmd.indexOf("\"ssid\":\"", dataStart) + 8;
        int ssidEnd = jsonCmd.indexOf("\"", ssidStart);
        
        // 提取密码
        int pwdStart = jsonCmd.indexOf("\"password\":\"", dataStart) + 12;
        int pwdEnd = jsonCmd.indexOf("\"", pwdStart);
        
        if (ssidStart > 8 && ssidEnd > ssidStart && pwdStart > 12 && pwdEnd > pwdStart) {
          String ssid = jsonCmd.substring(ssidStart, ssidEnd);
          String password = jsonCmd.substring(pwdStart, pwdEnd);
          
          Serial.println("提取的SSID: " + ssid);
          Serial.println("提取的密码: " + password);
          
          // 发送正在连接的状态
          String progressResponse = "{";
          progressResponse += "\"cmd\":\"set_wifi\",";
          progressResponse += "\"seq\":" + String(seq) + ",";
          progressResponse += "\"status\":\"connecting\",";
          progressResponse += "\"message\":\"正在连接WiFi...\"";
          progressResponse += "}";
          sendBluetoothData(progressResponse);
          
          // 使用try-catch块保护WiFi连接过程
          bool connected = false;
          try {
            // 确认WiFi连接函数存在
            if (WiFi.status() == WL_CONNECTED) {
              // 先断开现有连接
              WiFi.disconnect();
              delay(100);
            }
            
            // 设置WiFi模式为STA
            WiFi.mode(WIFI_STA);
            
            // 开始连接WiFi
            WiFi.begin(ssid.c_str(), password.c_str());
            
            // 等待连接，最多等待20秒
            int timeout = 20;
            while (WiFi.status() != WL_CONNECTED && timeout > 0) {
              delay(1000);
              Serial.print(".");
              timeout--;
            }
            
            connected = (WiFi.status() == WL_CONNECTED);
            
            if (!connected) {
              // 连接失败，关闭WiFi以节省电力
              WiFi.disconnect(true);
              WiFi.mode(WIFI_OFF);
            }
          } catch (...) {
            Serial.println("WiFi连接过程中发生异常");
            connected = false;
          }
          
          // 发送连接结果
          String response = "{";
          response += "\"cmd\":\"set_wifi\",";
          response += "\"seq\":" + String(seq) + ",";
          response += "\"status\":\"" + String(connected ? "success" : "failed") + "\",";
          response += "\"message\":\"" + String(connected ? "WiFi连接成功" : "WiFi连接失败") + "\"";
          if (connected) {
            response += ",\"ip\":\"" + WiFi.localIP().toString() + "\"";
          }
          response += "}";
          
          sendBluetoothData(response);
        } else {
          // SSID或密码解析失败
          String response = "{";
          response += "\"cmd\":\"set_wifi\",";
          response += "\"seq\":" + String(seq) + ",";
          response += "\"status\":\"failed\",";
          response += "\"message\":\"参数解析失败\"";
          response += "}";
          sendBluetoothData(response);
        }
      } else {
        // data字段解析失败
        String response = "{";
        response += "\"cmd\":\"set_wifi\",";
        response += "\"seq\":" + String(seq) + ",";
        response += "\"status\":\"failed\",";
        response += "\"message\":\"命令格式错误\"";
        response += "}";
        sendBluetoothData(response);
      }
    }
    // 可以根据需要添加更多JSON命令的处理逻辑
  }
}

/**
 * @brief 蓝牙命令处理
 * @param cmd 收到的命令字符串
 */
void processBluetoothCommand(String cmd) {
  cmd.trim();
  Serial.println("处理蓝牙命令: " + cmd);
  
  // 检查是否是连续的JSON字符串
  if (cmd.startsWith("{") && cmd.indexOf("}{") > 0) {
    Serial.println("检测到多个JSON命令");
    
    // 分割并处理每个JSON命令
    int startPos = 0;
    int endPos = 0;
    
    while (startPos < cmd.length()) {
      endPos = cmd.indexOf("}", startPos);
      if (endPos == -1) break;
      
      // 提取单个JSON命令
      String singleCmd = cmd.substring(startPos, endPos + 1);
      Serial.println("拆分JSON命令: " + singleCmd);
      
      // 处理JSON命令
      if (singleCmd.startsWith("{")) {
        processJsonCommand(singleCmd);
      }
      
      // 移动到下一个JSON命令的开始位置
      startPos = cmd.indexOf("{", endPos);
      if (startPos == -1) break;
    }
  }
  // 检查是否是单个JSON命令
  else if (cmd.startsWith("{") && cmd.endsWith("}")) {
    processJsonCommand(cmd);
  }
  // 处理传统格式的命令
  else if (cmd.startsWith("GET_TEMP")) {
    // 发送温湿度数据
    String response = "TEMP:" + String(dht30_temp) + "," + String(dht30_humi);
    sendBluetoothData(response);
  } 
  else if (cmd.startsWith("GET_BAT")) {
    // 发送电池电压
    String response = "BAT:" + String(bat_vcc);
    sendBluetoothData(response);
  } 
  else if (cmd.startsWith("REFRESH")) {
    // 请求刷新显示屏
    sendBluetoothData("CMD:REFRESH_START");
    // 此处可以调用刷新显示的函数
    sendBluetoothData("CMD:REFRESH_DONE");
  } 
  else if (cmd.startsWith("SLEEP")) {
    // 命令设备进入睡眠模式
    sendBluetoothData("CMD:SLEEP_START");
    // 进入睡眠前的操作可以在这里添加
  }
  else if (cmd.startsWith("SET_TIME:")) {
    // 设置时间，格式如 SET_TIME:2025-04-23 15:30:00
    String timeStr = cmd.substring(9);
    sendBluetoothData("TIME_SET:" + timeStr);
    // 这里可以添加设置RTC时间的代码
  }
  else if (cmd.startsWith("get_info")) {
    // 获取设备详细信息
    String mac = WiFi.macAddress();
    String ip = WiFi.localIP().toString();
    String wifiStatus = WiFi.status() == WL_CONNECTED ? "connected" : "disconnected";
    
    // 生成设备ID（从MAC地址创建）
    String deviceId = "ESP32-";
    // 取MAC地址的后半部分并去掉冒号
    for (int i = 9; i < mac.length(); i++) {
      if (mac.charAt(i) != ':') {
        deviceId += mac.charAt(i);
      }
    }
    
    // 创建JSON响应
    String response = "{";
    response += "\"cmd\":\"get_info\",";
    response += "\"seq\":1,";
    response += "\"data\":{";
    response += "\"device_id\":\"" + deviceId + "\",";
    response += "\"firmware_version\":\"1.0.0\",";
    response += "\"mac\":\"" + mac + "\",";
    response += "\"wifi_status\":\"" + wifiStatus + "\",";
    response += "\"ip\":\"" + ip + "\"";
    response += "}";
    response += "}";
    
    sendBluetoothData(response);
  }
  else {
    // 未知命令
    sendBluetoothData("ERR:UNKNOWN_COMMAND");
  }
}

/**
 * @brief 检查并处理蓝牙数据
 * 应在loop函数中定期调用
 * 
 * 注：本函数在使用BLE服务器回调的实现中变得不那么必要，
 * 但保留以便兼容现有代码逻辑和定期更新广播数据
 */
void handleBluetooth() {
  // 广播状态更新
  if (bleAdvertising && (millis() - lastAdvertisingUpdate > ADVERTISING_UPDATE_INTERVAL)) {
    updateBLEAdvertisingData();
  }
}

/**
 * @brief 获取蓝牙连接状态
 * @return 如果蓝牙已连接则返回true
 */
bool isBluetoothConnected() {
  return deviceConnected;
}

/**
 * @brief 停止蓝牙功能
 */
void stopBluetooth() {
  // 停止BLE
  if (pServer && bleAdvertising) {
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    if (pAdvertising) {
      pAdvertising->stop();
    }
    bleAdvertising = false;
    Serial.println("BLE广播已停止");
  }
  
  // 结束蓝牙设备
  BLEDevice::deinit(true);
  Serial.println("蓝牙已停止");
}

/**
 * @brief 强制更新广播数据
 * 当设备状态有重大变化时，可以调用此函数立即更新广播数据
 */
void forceUpdateAdvertising() {
  if (bleAdvertising && pServer) {
    updateBLEAdvertisingData();
  }
}
