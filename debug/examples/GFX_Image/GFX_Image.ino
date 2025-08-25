/*
 * @Description: GFX显示+触摸切换图片测试
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-09-06 10:58:19
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2025-08-25 15:33:41
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "Material_16Bit_280x456px.h"

static uint8_t Image_Flag = 0;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
    LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_CO5300(bus, LCD_RST /* RST */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT,
                                      20 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(LCD_EN, OUTPUT);
    digitalWrite(LCD_EN, HIGH);

    gfx->begin();
    gfx->fillScreen(RED);

    for (int i = 0; i <= 255; i++)
    {
        gfx->Display_Brightness(i);
        delay(3);
    }
}

void loop()
{
    gfx->fillScreen(RED);
    delay(3000);
    gfx->fillScreen(GREEN);
    delay(3000);
    gfx->fillScreen(BLUE);
    delay(3000);
    gfx->fillScreen(BLACK);
    delay(3000);
}
