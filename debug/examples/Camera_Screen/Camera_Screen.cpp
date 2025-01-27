/*
 * @Description: None
 * @version: V1.0.0
 * @Author: None
 * @Date: 2023-11-15 16:57:21
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-04-05 17:00:40
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include <esp_camera.h>
#include "camera_index.h"
#include "app_httpd.tpp"

bool OV2640_Initialization_Flag = false;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
    LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_CO5300(bus, LCD_RST /* RST */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT,
                                      20 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

bool OV2640_Initialization(void)
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_240X240;
    // config.pixel_format = PIXFORMAT_JPEG; // for streaming
    config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 0;
    config.fb_count = 2;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        gfx->printf("Camera init failed with error 0x%x \n", err);

        return false;
    }

    sensor_t *s = esp_camera_sensor_get();
    if (s)
    {
        Serial.print("camera id:");
        Serial.println(s->id.PID);
        gfx->print("camera id:");
        gfx->println(s->id.PID);
        gfx->println();
        camera_sensor_info_t *sinfo = esp_camera_sensor_get_info(&(s->id));
        if (sinfo)
        {
            Serial.print("camera model:");
            Serial.println(sinfo->name);
            gfx->print("camera model:");
            gfx->println(sinfo->name);
            gfx->println();
        }
    }
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);       // flip it back
        s->set_brightness(s, 1);  // up the brightness just a bit
        s->set_saturation(s, -2); // lower the saturation
    }

    return true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(LCD_EN, OUTPUT);
    digitalWrite(LCD_EN, HIGH);

    gfx->begin();
    gfx->fillScreen(WHITE);

    for (uint8_t i = 0; i < 255; i++)
    {
        gfx->Display_Brightness(i);
        delay(3);
    }

    gfx->setTextColor(BLACK);

    OV2640_Initialization_Flag = OV2640_Initialization();
}

void loop()
{
    if (OV2640_Initialization_Flag == true)
    {
        camera_fb_t *frame = esp_camera_fb_get();
        if (frame)
        {
            gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)frame->buf, frame->width, frame->height);
            esp_camera_fb_return(frame);
        }
        delay(1);
    }
}