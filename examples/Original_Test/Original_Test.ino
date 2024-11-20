/*
 * @Description: 出厂测试
 * @Author: LILYGO_L
 * @Date: 2023-09-06 10:58:19
 * @LastEditTime: 2024-11-20 10:44:49
 * @License: GPL 3.0
 */

#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "Wire.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include "Arduino_DriveBus_Library.h"
#include "Material_16Bit_280x456px.h"

#define WIFI_SSID "xinyuandianzi"
#define WIFI_PASSWORD "AA15994823428"
// #define WIFI_SSID "LilyGo-AABB"
// #define WIFI_PASSWORD "xinyuandianzi"

#define WIFI_CONNECT_WAIT_MAX (30000)

#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"
#define GMT_OFFSET_SEC 8 * 3600 // Time zone setting function, written as 8 * 3600 in East Eighth Zone (UTC/GMT+8:00)
#define DAY_LIGHT_OFFSET_SEC 0  // Fill in 3600 for daylight saving time, otherwise fill in 0

// 文件下载链接
// const char *fileDownloadUrl = "https://code.visualstudio.com/docs/?dv=win64user";//vscode
// const char *fileDownloadUrl = "https://dldir1.qq.com/qqfile/qq/PCQQ9.7.17/QQ9.7.17.29225.exe"; // 腾讯CDN加速
// const char *fileDownloadUrl = "https://cd001.www.duba.net/duba/install/packages/ever/kinsthomeui_150_15.exe"; // 金山毒霸
const char *fileDownloadUrl = "http://music.163.com/song/media/outer/url?id=26122999.mp3";
// const char *fileDownloadUrl = "https://github.com/espressif/arduino-esp32/releases/download/3.0.1/esp32-3.0.1.zip";

bool Wifi_Connection_Failure_Flag = false;

static size_t CycleTime = 0;
static size_t CycleTime_2 = 0;

static uint8_t Image_Flag = 0;

uint8_t OTG_Mode = 0;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
    LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_CO5300(bus, LCD_RST /* RST */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT,
                                      20 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> FT3168(new Arduino_FT3x68(IIC_Bus, FT3168_DEVICE_ADDRESS,
                                                       TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

std::unique_ptr<Arduino_IIC> SY6970(new Arduino_SY6970(IIC_Bus, SY6970_DEVICE_ADDRESS,
                                                       DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

void Arduino_IIC_Touch_Interrupt(void)
{
    FT3168->IIC_Interrupt_Flag = true;
}

void Wifi_STA_Test(void)
{
    String text;
    int wifi_num = 0;

    gfx->fillScreen(WHITE);
    gfx->setCursor(0, 0);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);

    Serial.println("\nScanning wifi");
    gfx->printf("Scanning wifi\n");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    delay(100);

    wifi_num = WiFi.scanNetworks();
    if (wifi_num == 0)
    {
        text = "\nWiFi scan complete !\nNo wifi discovered.\n";
    }
    else
    {
        text = "\nWiFi scan complete !\n";
        text += wifi_num;
        text += " wifi discovered.\n\n";

        for (int i = 0; i < wifi_num; i++)
        {
            text += (i + 1);
            text += ": ";
            text += WiFi.SSID(i);
            text += " (";
            text += WiFi.RSSI(i);
            text += ")";
            text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
            delay(10);
        }
    }

    Serial.println(text);
    gfx->println(text);

    delay(3000);
    text.clear();
    gfx->fillScreen(WHITE);
    gfx->setCursor(0, 10);

    text = "Connecting to ";
    Serial.print("Connecting to ");
    gfx->printf("Connecting to\n");
    text += WIFI_SSID;
    text += "\n";

    Serial.print(WIFI_SSID);
    gfx->printf("%s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t last_tick = millis();

    Wifi_Connection_Failure_Flag = false;
    while (WiFi.status() != WL_CONNECTED)
    {
        static uint8_t temp_count = 0;
        Serial.print(".");
        gfx->printf(".");
        text += ".";
        delay(100);

        temp_count++;
        if ((temp_count % 100) == 0)
        {
            temp_count = 0;
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        }

        if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
        {
            Wifi_Connection_Failure_Flag = true;
            temp_count = 0;
            break;
        }
    }

    if (!Wifi_Connection_Failure_Flag)
    {
        text += "\nThe connection was successful ! \nTakes ";
        Serial.print("\nThe connection was successful ! \nTakes ");
        gfx->printf("\nThe connection was successful ! \nTakes ");
        text += millis() - last_tick;
        Serial.print(millis() - last_tick);
        gfx->print(millis() - last_tick);
        text += " ms\n";
        Serial.println(" ms\n");
        gfx->printf(" ms\n");

        gfx->printf("\nWifi test passed!");
    }
    else
    {
        gfx->printf("\nWifi test error!\n");
    }
}

void PrintLocalTime(void)
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 3000))
    {
        Serial.println("Failed to obtain time");
        gfx->setCursor(50, 200);
        gfx->setTextColor(RED);
        gfx->print("Failed to obtain time!");
        return;
    }
    Serial.println("Get time success");
    Serial.println(&timeinfo, "%A,%B %d %Y %H:%M:%S"); // Format Output
    gfx->setCursor(50, 200);
    gfx->setTextColor(ORANGE);
    gfx->print(&timeinfo, " %Y");
    gfx->setCursor(50, 220);
    gfx->print(&timeinfo, "%B %d");
    gfx->setCursor(50, 240);
    gfx->print(&timeinfo, "%H:%M:%S");
}

void GFX_Print_Touch_Info_Loop(int32_t touch_x, int32_t touch_y, int32_t fingers_number)
{
    gfx->fillRect(30, 50, 250, 178, WHITE);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);

    gfx->setCursor(30, 50);
    gfx->printf("ID: %#X ", (int32_t)FT3168->IIC_Read_Device_ID());

    gfx->setCursor(30, 70);
    gfx->printf("Fingers Number:%d ", fingers_number);

    gfx->setCursor(30, 90);
    gfx->printf("Touch X:%d Y:%d ", touch_x, touch_y);
}

void GFX_Print_OTG_Switch_Info(uint8_t mode)
{
    switch (mode)
    {
    case 0:
        gfx->fillRect(50, 140, 170, 60, DARKGREY);
        gfx->setTextSize(2);
        gfx->setTextColor(WHITE);

        gfx->setCursor(100, 160);
        gfx->printf("OTG OFF");

        SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_OTG_MODE,
                                       SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_OFF);
        break;

    case 1:
        gfx->fillRect(50, 140, 170, 60, OLIVE);
        gfx->setTextSize(2);
        gfx->setTextColor(WHITE);

        gfx->setCursor(100, 160);
        gfx->printf("OTG ON");

        SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_OTG_MODE,
                                       SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON);
        break;

    default:
        break;
    }
}

void GFX_Print_Battery_Info_Loop()
{
    gfx->fillRect(0, 0, LCD_WIDTH, 256, WHITE);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);

    gfx->setCursor(30, 50);
    gfx->printf("Battery ADC ");

    gfx->setCursor(30, 90);
    gfx->printf("Charging Status: ");
    gfx->setCursor(30, 110);
    gfx->setTextColor(ORANGE);
    gfx->printf("%s ",
                (SY6970->IIC_Read_Device_State(SY6970->Arduino_IIC_Power::Status_Information::POWER_CHARGING_STATUS)).c_str());
    gfx->setTextColor(BLACK);

    gfx->setCursor(30, 170);
    gfx->printf("SY6970: %d mV",
                (uint32_t)SY6970->IIC_Read_Device_Value(SY6970->Arduino_IIC_Power::Value_Information::POWER_BATTERY_VOLTAGE));
    gfx->setCursor(30, 190);
    gfx->printf("Hardware: %d mV", analogReadMilliVolts(BATTERY_VOLTAGE_ADC_DATA) * 2);
}

void GFX_Print_Time_Info_Loop()
{
    gfx->fillRect(30, 30, 220, 120, WHITE);

    if (!Wifi_Connection_Failure_Flag)
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo, 3000))
        {
            Serial.println("Failed to obtain time");
            gfx->setCursor(50, 50);
            gfx->setTextColor(RED);
            gfx->setTextSize(2);
            gfx->print("Time error");
            return;
        }
        Serial.println("Get time success");
        Serial.println(&timeinfo, "%A,%B %d %Y %H:%M:%S"); // Format Output
        gfx->setCursor(50, 50);
        gfx->setTextColor(ORANGE);
        gfx->setTextSize(2);
        gfx->print(&timeinfo, " %Y");
        gfx->setCursor(50, 70);
        gfx->print(&timeinfo, "%B %d");
        gfx->setCursor(50, 90);
        gfx->print(&timeinfo, "%H:%M:%S");
    }
    else
    {
        gfx->setCursor(50, 50);
        gfx->setTextSize(2);
        gfx->setTextColor(RED);
        gfx->print("Network error");
    }

    gfx->setCursor(50, 110);
    gfx->printf("SYS Time:%d", (uint32_t)millis() / 1000);
}

void GFX_Print_1()
{
    gfx->fillRect(10, 300, 125, 50, ORANGE);
    gfx->drawRect(10, 300, 125, 50, PURPLE);
    gfx->fillRect(145, 300, 125, 50, PURPLE);
    gfx->drawRect(145, 300, 125, 50, ORANGE);
    gfx->setTextSize(2);
    gfx->setTextColor(WHITE);
    gfx->setCursor(20, 315);
    gfx->printf("Try Again");
    gfx->setCursor(155, 315);
    gfx->printf("Next Test");
}

void WIFI_HTTP_Download_File(void)
{
    // 初始化HTTP客户端
    HTTPClient http;
    http.begin(fileDownloadUrl);
    // 获取重定向的URL
    const char *headerKeys[] = {"Location"};
    http.collectHeaders(headerKeys, 1);

    // 记录下载开始时间
    size_t startTime = millis();
    // 无用时间
    size_t uselessTime = 0;

    // 发起GET请求
    int httpCode = http.GET();

    while (httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_FOUND)
    {
        String newUrl = http.header("Location");
        Serial.printf("Redirecting to: %s\n", newUrl.c_str());
        http.end(); // 关闭旧的HTTP连接

        // 使用新的URL重新发起GET请求
        http.begin(newUrl);
        httpCode = http.GET();
    }

    if (httpCode == HTTP_CODE_OK)
    {
        // 获取文件大小
        size_t fileSize = http.getSize();
        Serial.printf("Starting file download...\n");
        Serial.printf("file size: %f MB\n", fileSize / 1024.0 / 1024.0);
        gfx->setCursor(0, 0);
        gfx->printf("Starting file download...\n");
        gfx->printf("file size: %f MB\n", fileSize / 1024.0 / 1024.0);

        // 读取HTTP响应
        WiFiClient *stream = http.getStreamPtr();

        size_t temp_count_s = 0;
        bool temp_count_flag = true;
        size_t temp_fileSize = fileSize;
        // uint8_t *buf_1 = (uint8_t *)heap_caps_malloc(64 * 1024, MALLOC_CAP_SPIRAM);
        uint8_t buf_1[4096] = {0};
        CycleTime = millis() + 3000; // 开始计时
        while (http.connected() && (temp_fileSize > 0 || temp_fileSize == -1))
        {
            // 获取可用数据的大小
            size_t availableSize = stream->available();
            if (availableSize)
            {
                // temp_fileSize -= stream->read(buf_1, min(availableSize, (size_t)(64 * 1024)));
                temp_fileSize -= stream->read(buf_1, min(availableSize, (size_t)(4096)));

                if (millis() > CycleTime)
                {
                    size_t temp_time_1 = millis();
                    temp_count_s += 3;
                    Serial.printf("WIFI RSSI: (%d)\n", WiFi.RSSI());
                    Serial.printf("Download speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / temp_count_s);
                    Serial.printf("Remaining file size: %f MB\n\n", temp_fileSize / 1024.0 / 1024.0);

                    gfx->fillRect(0, 100, 400, 68, WHITE);
                    gfx->setCursor(0, 100);
                    gfx->printf("WIFI RSSI: (%d)\n", WiFi.RSSI());
                    gfx->printf("Speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / temp_count_s);
                    gfx->printf("Size: %f MB\n\n", temp_fileSize / 1024.0 / 1024.0);

                    CycleTime = millis() + 3000;
                    size_t temp_time_2 = millis();

                    uselessTime = uselessTime + (temp_time_2 - temp_time_1);
                }
            }

            if (temp_count_s > 30)
            {
                temp_count_flag = false;
                break;
            }
        }

        // 关闭HTTP客户端
        http.end();

        // 记录下载结束时间并计算总花费时间
        size_t endTime = millis();

        if (temp_count_flag == true)
        {
            Serial0.printf("Download completed!\n");
            Serial0.printf("Total download time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
            Serial0.printf("Average download speed: %f KB/s\n", (fileSize / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));

            gfx->printf("Completed!\n");
            gfx->printf("Time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
            gfx->printf("Speed: %f KB/s\n", (fileSize / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));
        }
        else
        {
            Serial0.printf("Download incomplete!\n");
            Serial0.printf("Download time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
            Serial0.printf("Average download speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));

            gfx->printf("Incompleted!\n");
            gfx->printf("Time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
            gfx->printf("Speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));
        }
    }
    else
    {
        Serial.printf("Failed to download\n");
        Serial.printf("Error httpCode: %d \n", httpCode);

        gfx->printf("Failed to download\n");
        gfx->printf("Error httpCode: %d \n\n", httpCode);
    }
}

void Original_Test_1()
{
    gfx->fillScreen(WHITE);
    gfx->setCursor(90, 130);
    gfx->setTextSize(4);
    gfx->setTextColor(PINK);
    gfx->printf("TEST");

    gfx->setCursor(10, 170);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);
    gfx->printf("1.Touch Test");

    gfx->setCursor(120, 250);
    gfx->setTextSize(6);
    gfx->setTextColor(RED);
    gfx->printf("3");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("2");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("1");
    delay(1000);

    gfx->fillScreen(WHITE);

    int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
    int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
    uint8_t fingers_number = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

    GFX_Print_Touch_Info_Loop(touch_x, touch_y, fingers_number);

    GFX_Print_1();
}

void Original_Test_2()
{
    gfx->fillScreen(WHITE);
    gfx->setCursor(90, 130);
    gfx->setTextSize(4);
    gfx->setTextColor(PINK);
    gfx->printf("TEST");

    gfx->setCursor(10, 170);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);
    gfx->printf("2.OLED Edge Detection Test");

    gfx->setCursor(120, 250);
    gfx->setTextSize(6);
    gfx->setTextColor(RED);
    gfx->printf("3");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("2");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("1");
    delay(1000);

    gfx->fillScreen(WHITE);
    gfx->drawRect(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, RED);

    gfx->setCursor(70, 160);
    gfx->setTextSize(4);
    gfx->setTextColor(ORANGE);
    gfx->printf("FINISH");

    GFX_Print_1();
}

void Original_Test_3()
{
    gfx->fillScreen(WHITE);
    gfx->setCursor(90, 130);
    gfx->setTextSize(4);
    gfx->setTextColor(PINK);
    gfx->printf("TEST");

    gfx->setCursor(10, 170);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);
    gfx->printf("3.OLED Backlight Test");

    gfx->setCursor(120, 250);
    gfx->setTextSize(6);
    gfx->setTextColor(RED);
    gfx->printf("3");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("2");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("1");
    delay(1000);

    gfx->fillScreen(WHITE);
    gfx->setCursor(80, 160);
    gfx->setTextSize(4);
    gfx->setTextColor(RED);
    gfx->printf("START");

    for (int i = 255; i > 0; i--)
    {
        gfx->Display_Brightness(i);
        delay(2);
    }
    delay(3000);
    for (int i = 0; i <= 255; i++)
    {
        gfx->Display_Brightness(i);
        delay(5);
    }

    delay(1000);

    gfx->fillScreen(WHITE);
    gfx->setCursor(70, 160);
    gfx->setTextSize(4);
    gfx->setTextColor(ORANGE);
    gfx->printf("FINISH");

    GFX_Print_1();
}

void Original_Test_4()
{
    gfx->fillScreen(WHITE);
    gfx->setCursor(90, 130);
    gfx->setTextSize(4);
    gfx->setTextColor(PINK);
    gfx->printf("TEST");

    gfx->setCursor(10, 170);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);
    gfx->printf("4.OLED Color Test");

    gfx->setCursor(120, 250);
    gfx->setTextSize(6);
    gfx->setTextColor(RED);
    gfx->printf("3");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("2");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("1");
    delay(1000);

    gfx->fillScreen(RED);
    delay(3000);
    gfx->fillScreen(GREEN);
    delay(3000);
    gfx->fillScreen(BLUE);
    delay(3000);

    gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, LCD_WIDTH, LCD_HEIGHT);
    delay(3000);

    gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, LCD_WIDTH, LCD_HEIGHT);
    delay(3000);

    gfx->setCursor(70, 160);
    gfx->setTextSize(4);
    gfx->setTextColor(ORANGE);
    gfx->printf("FINISH");

    GFX_Print_1();
}

void Original_Test_5()
{
    gfx->fillScreen(WHITE);
    gfx->setCursor(90, 130);
    gfx->setTextSize(4);
    gfx->setTextColor(PINK);
    gfx->printf("TEST");

    gfx->setCursor(10, 170);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);
    gfx->printf("5.OTG Test");

    gfx->setCursor(120, 250);
    gfx->setTextSize(6);
    gfx->setTextColor(RED);
    gfx->printf("3");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("2");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("1");
    delay(1000);

    gfx->fillScreen(WHITE);

    GFX_Print_OTG_Switch_Info(0);
    GFX_Print_1();
}

void Original_Test_6()
{
    // 关闭OTG测试
    SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_OTG_MODE,
                                   SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_OFF);
    // 开启充电
    SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
                                   SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON);

    gfx->fillScreen(WHITE);
    gfx->setCursor(90, 130);
    gfx->setTextSize(4);
    gfx->setTextColor(PINK);
    gfx->printf("TEST");

    gfx->setCursor(10, 170);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);
    gfx->printf("6.Battery Voltage Detection Test");

    gfx->setCursor(120, 250);
    gfx->setTextSize(6);
    gfx->setTextColor(RED);
    gfx->printf("3");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("2");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("1");
    delay(1000);

    gfx->fillScreen(WHITE);

    GFX_Print_1();
}

void Original_Test_7()
{
    gfx->fillScreen(WHITE);
    gfx->setCursor(90, 130);
    gfx->setTextSize(4);
    gfx->setTextColor(PINK);
    gfx->printf("TEST");

    gfx->setCursor(10, 170);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);
    gfx->printf("7.WIFI STA Test");

    gfx->setCursor(120, 250);
    gfx->setTextSize(6);
    gfx->setTextColor(RED);
    gfx->printf("3");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("2");
    delay(1000);
    gfx->fillRect(120, 250, 150, 200, WHITE);
    gfx->setCursor(120, 250);
    gfx->printf("1");
    delay(1000);

    Wifi_STA_Test();

    delay(2000);

    gfx->fillScreen(WHITE);

    if (!Wifi_Connection_Failure_Flag)
    {
        // Obtain and set the time from the network time server
        // After successful acquisition, the chip will use the RTC clock to update the holding time
        configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);

        WIFI_HTTP_Download_File();
        // delay(3000);

        // PrintLocalTime();
    }
    else
    {
        gfx->setCursor(20, 100);
        gfx->setTextColor(RED);
        gfx->print("Not connected to the network");
    }

    // gfx->fillScreen(WHITE);

    // gfx->setCursor(70, 160);
    // gfx->setTextSize(4);
    // gfx->setTextColor(ORANGE);
    // gfx->printf("FINISH");

    GFX_Print_1();
}

void Original_Test_Loop()
{
    Original_Test_1();

    while (1)
    {
        bool temp = false;

        if (FT3168->IIC_Interrupt_Flag == true)
        {
            FT3168->IIC_Interrupt_Flag = false;

            int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            GFX_Print_Touch_Info_Loop(touch_x, touch_y, fingers_number);

            if (fingers_number > 0)
            {
                if (touch_x > 10 && touch_x < 135 && touch_y > 300 && touch_y < 350)
                {
                    Original_Test_1();
                }
                if (touch_x > 145 && touch_x < 280 && touch_y > 300 && touch_y < 350)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_2();

    while (1)
    {
        bool temp = false;

        if (FT3168->IIC_Interrupt_Flag == true)
        {
            FT3168->IIC_Interrupt_Flag = false;

            int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 10 && touch_x < 135 && touch_y > 300 && touch_y < 350)
                {
                    Original_Test_2();
                }
                if (touch_x > 145 && touch_x < 280 && touch_y > 300 && touch_y < 350)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_3();

    while (1)
    {
        bool temp = false;

        if (FT3168->IIC_Interrupt_Flag == true)
        {
            FT3168->IIC_Interrupt_Flag = false;

            int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 10 && touch_x < 135 && touch_y > 300 && touch_y < 350)
                {
                    Original_Test_3();
                }
                if (touch_x > 145 && touch_x < 280 && touch_y > 300 && touch_y < 350)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_4();

    while (1)
    {
        bool temp = false;

        if (FT3168->IIC_Interrupt_Flag == true)
        {
            FT3168->IIC_Interrupt_Flag = false;

            int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 10 && touch_x < 135 && touch_y > 300 && touch_y < 350)
                {
                    Original_Test_4();
                }
                if (touch_x > 145 && touch_x < 280 && touch_y > 300 && touch_y < 350)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_5();

    while (1)
    {
        bool temp = false;

        if (FT3168->IIC_Interrupt_Flag == true)
        {
            FT3168->IIC_Interrupt_Flag = false;

            int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 10 && touch_x < 135 && touch_y > 300 && touch_y < 350)
                {
                    Original_Test_5();
                }
                if (touch_x > 145 && touch_x < 280 && touch_y > 300 && touch_y < 350)
                {
                    temp = true;
                }
                if (touch_x > 50 && touch_x < 220 && touch_y > 140 && touch_y < 200)
                {
                    OTG_Mode = !OTG_Mode;
                    GFX_Print_OTG_Switch_Info(OTG_Mode);
                    delay(300);
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_6();

    while (1)
    {
        bool temp = false;

        if (millis() > CycleTime)
        {
            GFX_Print_Battery_Info_Loop();
            CycleTime = millis() + 1000;
        }

        if (FT3168->IIC_Interrupt_Flag == true)
        {
            FT3168->IIC_Interrupt_Flag = false;

            int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 10 && touch_x < 135 && touch_y > 300 && touch_y < 350)
                {
                    Original_Test_6();
                }
                if (touch_x > 145 && touch_x < 280 && touch_y > 300 && touch_y < 350)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_7();

    while (1)
    {
        bool temp = false;

        if (FT3168->IIC_Interrupt_Flag == true)
        {
            FT3168->IIC_Interrupt_Flag = false;

            int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 10 && touch_x < 135 && touch_y > 300 && touch_y < 350)
                {
                    Original_Test_7();
                }
                if (touch_x > 145 && touch_x < 280 && touch_y > 300 && touch_y < 350)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(LCD_EN, OUTPUT);
    digitalWrite(LCD_EN, HIGH);

    pinMode(BATTERY_VOLTAGE_ADC_DATA, INPUT_PULLDOWN);
    analogReadResolution(12);
    analogSetPinAttenuation(BATTERY_VOLTAGE_ADC_DATA, ADC_ATTENDB_MAX);
    adcAttachPin(BATTERY_VOLTAGE_ADC_DATA);

    if (SY6970->begin() == false)
    {
        Serial.println("SY6970 initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("SY6970 initialization successfully");
    }

    // 开启ADC测量功能
    if (SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_ADC_MEASURE,
                                       SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON) == false)
    {
        Serial.println("SY6970 ADC Measure ON fail");
        delay(2000);
    }
    else
    {
        Serial.println("SY6970 ADC Measure ON successfully");
    }
    // 禁用看门狗定时器喂狗功能
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_WATCHDOG_TIMER, 0);
    // 热调节阈值设置为60度
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_THERMAL_REGULATION_THRESHOLD, 60);
    // 充电目标电压电压设置为4224mV
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_CHARGING_TARGET_VOLTAGE_LIMIT, 4224);
    // 最小系统电压限制为3600mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_MINIMUM_SYSTEM_VOLTAGE_LIMIT, 3600);
    // 设置OTG电压为5062mV
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_OTG_VOLTAGE_LIMIT, 5062);
    // 输入电流限制设置为600mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_INPUT_CURRENT_LIMIT, 600);
    // 快速充电电流限制设置为2112mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_FAST_CHARGING_CURRENT_LIMIT, 2112);
    // 预充电电流限制设置为192mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_PRECHARGE_CHARGING_CURRENT_LIMIT, 192);
    // 终端充电电流限制设置为320mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_TERMINATION_CHARGING_CURRENT_LIMIT, 320);
    // OTG电流限制设置为500mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_OTG_CHARGING_LIMIT, 500);

    if (FT3168->begin() == false)
    {
        Serial.println("FT3168 initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("FT3168 initialization successfully");
    }
    Serial.printf("ID: %#X \n\n", (int32_t)FT3168->IIC_Read_Device_ID());

    gfx->begin();
    gfx->fillScreen(BLACK);

    for (int i = 0; i <= 255; i++)
    {
        gfx->Display_Brightness(i);
        delay(3);
    }

    Original_Test_Loop();

    gfx->fillScreen(PINK);
}

void loop()
{
    if (millis() > CycleTime_2)
    {
        GFX_Print_Time_Info_Loop();
        CycleTime_2 = millis() + 1000;
    }

    if (FT3168->IIC_Interrupt_Flag == true)
    {
        FT3168->IIC_Interrupt_Flag = false;

        int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
        int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
        uint8_t fingers_number = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

        if (fingers_number > 0)
        {
            switch (Image_Flag)
            {
            case 0:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, LCD_WIDTH, LCD_HEIGHT); // RGB
                break;
            case 1:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, LCD_WIDTH, LCD_HEIGHT); // RGB
                break;
            case 2:
                gfx->fillScreen(PINK);
                gfx->setCursor(30, 250);
                gfx->setTextColor(YELLOW);
                gfx->setTextSize(2);
                gfx->println("Ciallo1~(L *##*L)^**");
                break;

            default:
                break;
            }

            Image_Flag++;

            if (Image_Flag > 2)
            {
                Image_Flag = 0;
            }

            Serial.printf("[1] point x: %d  point y: %d \r\n", touch_x, touch_y);

            gfx->setCursor(touch_x, touch_y);
            gfx->setTextColor(RED);
            gfx->printf("[1] point x: %d  point y: %d \r\n", touch_x, touch_y);
        }
    }
}
