/**
 * SD卡管理模块
 * 负责处理SD卡的初始化、读写操作等
 * 引脚配置在config.h中定义
 */

#include "config.h"
#include <SD.h>
#include <SPI.h>

SPIClass sdSPI(VSPI);   // 使用VSPI接口

// SD卡状态标志
bool sdCardMounted = false;

/**
 * 初始化SD卡
 * 返回值: true=初始化成功，false=初始化失败
 */
bool initSDCard() {
  Serial.println("正在初始化SD卡...");
  
  // 配置SPI总线用于SD卡
  sdSPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  
  // 尝试挂载SD卡
  if (!SD.begin(SD_CS_PIN, sdSPI)) {
    Serial.println("SD卡初始化失败!");
    return false;
  }
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("未检测到SD卡!");
    return false;
  }
  
  sdCardMounted = true;
  
  Serial.print("SD卡类型: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("未知类型");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD卡容量: %lluMB\n", cardSize);
  
  return true;
}

/**
 * 检查SD卡是否已经挂载
 * 返回值: true=已挂载，false=未挂载
 */
bool isSDCardMounted() {
  return sdCardMounted;
}

/**
 * 读取SD卡中的文本文件
 * 参数: filePath - 文件路径
 * 返回值: 文件内容字符串，如果读取失败则返回空字符串
 */
String readTextFile(const char* filePath) {
  if (!sdCardMounted) {
    Serial.println("SD卡未挂载!");
    return "";
  }
  
  File file = SD.open(filePath, FILE_READ);
  if (!file) {
    Serial.printf("无法打开文件: %s\n", filePath);
    return "";
  }
  
  String content = "";
  while (file.available()) {
    content += (char)file.read();
  }
  
  file.close();
  return content;
}

/**
 * 写入文本到SD卡
 * 参数:
 *   filePath - 文件路径
 *   content - 要写入的内容
 *   append - 是否追加到文件末尾(true)或覆盖(false)
 * 返回值: true=写入成功，false=写入失败
 */
bool writeTextFile(const char* filePath, const String& content, bool append = false) {
  if (!sdCardMounted) {
    Serial.println("SD卡未挂载!");
    return false;
  }
  
  File file = SD.open(filePath, append ? FILE_APPEND : FILE_WRITE);
  if (!file) {
    Serial.printf("无法打开文件用于写入: %s\n", filePath);
    return false;
  }
  
  size_t bytesWritten = file.print(content);
  file.close();
  
  if (bytesWritten == 0) {
    Serial.println("写入文件失败!");
    return false;
  }
  
  return true;
}

/**
 * 列出SD卡中指定目录下的所有文件
 * 参数: dirPath - 目录路径，默认为根目录
 */
void listDirectory(const char* dirPath = "/") {
  if (!sdCardMounted) {
    Serial.println("SD卡未挂载!");
    return;
  }
  
  File root = SD.open(dirPath);
  if (!root) {
    Serial.printf("无法打开目录: %s\n", dirPath);
    return;
  }
  
  if (!root.isDirectory()) {
    Serial.printf("%s 不是一个目录\n", dirPath);
    return;
  }
  
  Serial.printf("列出目录: %s\n", dirPath);
  
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("  DIR : %s\n", file.name());
    } else {
      Serial.printf("  FILE: %s (%u bytes)\n", file.name(), file.size());
    }
    file = root.openNextFile();
  }
  
  root.close();
}

/**
 * 检查文件是否存在
 * 参数: filePath - 文件路径
 * 返回值: true=文件存在，false=文件不存在
 */
bool fileExists(const char* filePath) {
  if (!sdCardMounted) {
    return false;
  }
  return SD.exists(filePath);
}

/**
 * 删除文件
 * 参数: filePath - 文件路径
 * 返回值: true=删除成功，false=删除失败
 */
bool deleteFile(const char* filePath) {
  if (!sdCardMounted) {
    Serial.println("SD卡未挂载!");
    return false;
  }
  
  if (SD.remove(filePath)) {
    Serial.printf("文件已删除: %s\n", filePath);
    return true;
  } else {
    Serial.printf("删除文件失败: %s\n", filePath);
    return false;
  }
}

/**
 * 创建目录
 * 参数: dirPath - 目录路径
 * 返回值: true=创建成功，false=创建失败
 */
bool createDirectory(const char* dirPath) {
  if (!sdCardMounted) {
    Serial.println("SD卡未挂载!");
    return false;
  }
  
  if (SD.mkdir(dirPath)) {
    Serial.printf("目录已创建: %s\n", dirPath);
    return true;
  } else {
    Serial.printf("创建目录失败: %s\n", dirPath);
    return false;
  }
}

/**
 * 获取SD卡剩余空间(KB)
 * 返回值: 剩余空间，单位KB
 */
uint64_t getSDCardFreeSpace() {
  if (!sdCardMounted) {
    return 0;
  }
  
  return SD.cardSize() - SD.usedBytes();
}

/**
 * 读取二进制文件内容到缓冲区
 * 参数:
 *   filePath - 文件路径
 *   buffer - 目标缓冲区
 *   maxLen - 最大读取长度
 * 返回值: 实际读取的字节数，失败返回-1
 */
int readBinaryFile(const char* filePath, uint8_t* buffer, size_t maxLen) {
  if (!sdCardMounted || !buffer) {
    return -1;
  }
  
  File file = SD.open(filePath, FILE_READ);
  if (!file) {
    return -1;
  }
  
  size_t bytesRead = file.read(buffer, maxLen);
  file.close();
  
  return bytesRead;
}

/**
 * 写入二进制数据到文件
 * 参数:
 *   filePath - 文件路径
 *   buffer - 数据缓冲区
 *   len - 数据长度
 *   append - 是否追加到文件末尾(true)或覆盖(false)
 * 返回值: true=写入成功，false=写入失败
 */
bool writeBinaryFile(const char* filePath, const uint8_t* buffer, size_t len, bool append = false) {
  if (!sdCardMounted || !buffer) {
    return false;
  }
  
  File file = SD.open(filePath, append ? FILE_APPEND : FILE_WRITE);
  if (!file) {
    return false;
  }
  
  size_t bytesWritten = file.write(buffer, len);
  file.close();
  
  return (bytesWritten == len);
}
