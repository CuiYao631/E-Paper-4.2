
#include "config.h"

void BW_refresh() // 黑白刷新一次
{
  display.init(0, 0, 10, 0);
  display.fillScreen(heise); // 填充屏幕
  display.display(1);        // 显示缓冲内容到屏幕，用于全屏缓冲
  display.fillScreen(baise);
  display.display(1);
}

/**
 * 绘制居中文本
 * @param text 要显示的文本内容
 * @param x 文本中心的x坐标
 * @param y 文本中心的y坐标
 * @param isInverted 是否反转颜色(黑底白字)
 */
void drawCenteredText(const char *text, int16_t x, int16_t y, bool isInverted)
{
  // 获取文本宽度
  int16_t textWidth = u8g2Fonts.getUTF8Width(text);

  // 计算居中位置
  int16_t x1 = x - textWidth / 2;
  int16_t y1 = y;

  // 设置颜色（正常或反转）
  if (isInverted)
  {
    u8g2Fonts.setForegroundColor(baise);
    u8g2Fonts.setBackgroundColor(heise);
  }
  else
  {
    u8g2Fonts.setForegroundColor(heise);
    u8g2Fonts.setBackgroundColor(baise);
  }

  // 绘制文本
  u8g2Fonts.setCursor(x1, y1);
  u8g2Fonts.print(text);

  // 恢复默认颜色
  u8g2Fonts.setForegroundColor(heise);
  u8g2Fonts.setBackgroundColor(baise);
}

/**
 * 绘制开机进度条
 * 自动居中显示进度条和状态文字
 * @param progress 当前进度(0-100)
 * @param x 水平偏移(默认应为0)
 * @param str 要显示的状态文字
 */
void onProgressBar(int progress, int x, char *str)
{
  // 计算进度条的水平居中位置
  int16_t progressBarX = (display.width() - PROGRESS_BAR_WIDTH) / 2;

  // 绘制进度条背景
  display.drawRect(progressBarX, PROGRESS_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, GxEPD_BLACK);

  // 设置进度条填充颜色
  uint16_t progressColor = GxEPD_BLACK;
  // 计算填充矩形的宽度
  int16_t progressWidth = map(progress, 0, 100, 0, PROGRESS_BAR_WIDTH);
  // 绘制进度条填充矩形
  display.fillRect(progressBarX, PROGRESS_BAR_Y, progressWidth, PROGRESS_BAR_HEIGHT, progressColor);

  // 计算文字宽度以居中显示
  const char *character = str;
  uint16_t textWidth = u8g2Fonts.getUTF8Width(character); // 获取文字像素宽度
  // 计算文字居中的X坐标 = 进度条起始X + (进度条宽度 - 文字宽度)/2
  uint16_t textX = progressBarX + (PROGRESS_BAR_WIDTH - textWidth) / 2;

  // 清除上一次的文字区域
  display.fillRect(progressBarX, PROGRESS_BAR_Y + 20, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, baise);

  // 设置开机加载文字在居中位置
  u8g2Fonts.setCursor(textX, PROGRESS_BAR_Y + 35);
  u8g2Fonts.println(str);

  // 更新显示
  display.display(true);

  // 等待一段时间
  delay(200);
}
