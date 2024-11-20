#include <Arduino.h>
#include "pin_config.h"

#define WC write_cmd
#define WD write_dat

#define COL LCD_WIDTH
#define ROW LCD_HEIGHT

void spi_write(unsigned char dat) // 发送8bit
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        if (dat & 0x80)
        {
            digitalWrite(LCD_SDIO0, HIGH);
        }
        else
        {
            digitalWrite(LCD_SDIO0, LOW);
        }

        dat <<= 1;

        digitalWrite(LCD_SCLK, LOW);
        // delayMicroseconds(4);
        digitalWrite(LCD_SCLK, HIGH);
    }
}

void write_dat(unsigned char dat)
{
    spi_write(dat);
}

void write_cmd(unsigned char cmd)
{
    spi_write(0x02);
    spi_write(0x00);
    spi_write(cmd);
    spi_write(0x00);
    // delayMicroseconds(2);
}

//=======SPI初始化代码==================
void LCD_Init(void)
{
    digitalWrite(LCD_RST, HIGH);
    delay(10);
    digitalWrite(LCD_RST, LOW);
    delay(200);
    digitalWrite(LCD_RST, HIGH);
    delay(200);

    // write_cmd(0xFE);
    // write_dat(0x01);
    // digitalWrite(LCD_CS, HIGH);
    // delayMicroseconds(4);

    // inital code version date: 211026
    // Truly1.78"+SH8601A RM20013

    digitalWrite(LCD_CS, HIGH);
    digitalWrite(LCD_CS, LOW);
    WC(0x11);
    delay(120);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    WC(0xFE);
    WD(0x00);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    WC(0xC4);
    WD(0x80);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    WC(0x3A);
    WD(0x55);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    WC(0x35);
    WD(0x00);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    WC(0x53);
    WD(0x20);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    WC(0x63);
    WD(0xFF);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    WC(0x2A);
    WD(0x00);
    WD(0x14);
    WD(0x01);
    WD(0x2B);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    WC(0x2B);
    WD(0x00);
    WD(0x00);
    WD(0x01);
    WD(0xC7);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    WC(0x29);
    digitalWrite(LCD_CS, HIGH);
    // delay(10);
    digitalWrite(LCD_CS, LOW);
    WC(0x51);
    WD(0xFF);
    digitalWrite(LCD_CS, HIGH);
}
//=====SPI end================================

void BlockWrite(unsigned int Xstart, unsigned int Xend, unsigned int Ystart, unsigned int Yend)
{
    Xstart += 20;
    Xend += 20;

    digitalWrite(LCD_CS, LOW);
    write_cmd(0x2a);
    write_dat(Xstart >> 8);
    write_dat(Xstart & 0xff);
    write_dat(Xend >> 8);
    write_dat(Xend & 0xff);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    write_cmd(0x2b);
    write_dat(Ystart >> 8);
    write_dat(Ystart & 0xff);
    write_dat(Yend >> 8);
    write_dat(Yend & 0xff);
    digitalWrite(LCD_CS, HIGH);

    digitalWrite(LCD_CS, LOW);
    write_cmd(0x2C);
    digitalWrite(LCD_CS, HIGH);
}

void write_lcd(unsigned char dat) // 4-wire Pixel Write Data Waveform
{
    digitalWrite(LCD_SCLK, LOW);
    // delayMicroseconds(2);

    if (dat & 0x80)
    {
        digitalWrite(LCD_SDIO3, HIGH);
    } // bit7
    else
    {
        digitalWrite(LCD_SDIO3, LOW);
    }

    if (dat & 0x40)
    {
        digitalWrite(LCD_SDIO2, HIGH);
    } // bit6
    else
    {
        digitalWrite(LCD_SDIO2, LOW);
    }

    if (dat & 0x20)
    {
        digitalWrite(LCD_SDIO1, HIGH);
    } // bit5
    else
    {
        digitalWrite(LCD_SDIO1, LOW);
    }

    if (dat & 0x10)
    {
        digitalWrite(LCD_SDIO0, HIGH);
    } // bit4
    else
    {
        digitalWrite(LCD_SDIO0, LOW);
    }

    digitalWrite(LCD_SCLK, HIGH);
    // delayMicroseconds(2);

    digitalWrite(LCD_SCLK, LOW);
    // delayMicroseconds(2);

    if (dat & 0x08)
    {
        digitalWrite(LCD_SDIO3, HIGH);
    } // bit3
    else
    {
        digitalWrite(LCD_SDIO3, LOW);
    }

    if (dat & 0x04)
    {
        digitalWrite(LCD_SDIO2, HIGH);
    } // bit2
    else
    {
        digitalWrite(LCD_SDIO2, LOW);
    }

    if (dat & 0x02)
    {
        digitalWrite(LCD_SDIO1, HIGH);
    } // bit1
    else
    {
        digitalWrite(LCD_SDIO1, LOW);
    }

    if (dat & 0x01)
    {
        digitalWrite(LCD_SDIO0, HIGH);

    } // bit0
    else
    {
        digitalWrite(LCD_SDIO0, LOW);
    }

    digitalWrite(LCD_SCLK, HIGH);
    // delayMicroseconds(2);
}

void shizi_mark()
{
    unsigned int i, j;

    BlockWrite(0, COL - 1, 0, ROW - 1);

    digitalWrite(LCD_CS, LOW);

    spi_write(0x12);
    write_lcd(0x00);
    write_lcd(0x2C);
    write_lcd(0x00);

    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL; j++)
        {
            if (i == ROW / 2 - 1 || i == ROW / 2 || j == COL / 2 - 1 || j == COL / 2)
            {
                write_lcd(0xf8);
                write_lcd(0x00);
            }
            else
            {
                write_lcd(0xFF);
                write_lcd(0xFF);
            }
        }
    }

    digitalWrite(LCD_CS, HIGH);
}

void RGB(void)
{
    unsigned int i, j, k;
    unsigned char color[] = {0xf8, 0x00, 0x07, 0xe0, 0x00, 0x1f};

    BlockWrite(0, COL - 1, 0, ROW - 1);

    digitalWrite(LCD_CS, LOW);

    spi_write(0x32);
    spi_write(0x00);
    spi_write(0x3C);
    spi_write(0x00);

    for (i = 0; i < 3; i++)
    {
        for (k = 0; k < ROW / 3; k++)
        {
            for (j = 0; j < COL; j++)
            {
                write_lcd(color[2 * i]);
                write_lcd(color[2 * i + 1]);
            }
        }
    }

    for (k = 0; k < ROW % 3; k++)
    {
        for (j = 0; j < COL; j++)
        {
            write_lcd(color[4]);
            write_lcd(color[5]);
        }
    }

    digitalWrite(LCD_CS, HIGH);
}

void frame()
{
    unsigned int i, j;

    BlockWrite(0, COL - 1, 0, ROW - 1);

    digitalWrite(LCD_CS, LOW);

    spi_write(0x12);
    write_lcd(0x00);
    write_lcd(0x2C);
    write_lcd(0x00);

    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL; j++)
        {
            if (i == 0 || i == ROW - 1 || j == 0 || j == COL - 1)
            {
                write_lcd(0xf8);
                write_lcd(0x00);
            }
            else
            {
                write_lcd(0x00);
                write_lcd(0x00);
            }
        }
    }

    digitalWrite(LCD_CS, HIGH);
}

void gray16(void)
{
    unsigned int i, j, k;
    unsigned char grayH8, grayL8;

    BlockWrite(0, COL - 1, 0, ROW - 1);

    digitalWrite(LCD_CS, LOW);

    spi_write(0x12);
    write_lcd(0x00);
    write_lcd(0x2C);
    write_lcd(0x00);

    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL % 16; j++)
        {
            write_lcd(0x00);
            write_lcd(0x00);
        }

        // GRAY
        for (j = 0; j < 16; j++)
        {
            grayH8 = ((j * 2) << 3) | ((j * 4) >> 3);
            grayL8 = ((j * 4) << 5) | (j * 2);

            for (k = 0; k < COL / 16; k++)
            {
                write_lcd(grayH8);
                write_lcd(grayL8);
            }
        }
    }

    digitalWrite(LCD_CS, HIGH);
}

void Gray_bar_H16(void) // 灰阶横条16
{
    uint16_t i, j, t;
    uint16_t Gray_strip[16] = {
        0x0000, 0x1082, 0x2104, 0x3186, 0x4208, 0x528a, 0x630c, 0x738e,
        0x8410, 0x9492, 0xa514, 0xb596, 0xc618, 0xd69a, 0xef7d, 0xffff};

    BlockWrite(0, COL - 1, 0, ROW - 1);

    digitalWrite(LCD_CS, LOW);

    spi_write(0x32);
    spi_write(0x00);
    spi_write(0x2C);
    spi_write(0x00);

    for (i = 0; i < 16; i++)
        for (j = 0; j < ROW / 16; j++)
            for (t = 0; t < COL; t++)
            {
                write_lcd(Gray_strip[i] >> 8);
                write_lcd(Gray_strip[i]);
            }

    digitalWrite(LCD_CS, HIGH);
}

void DispColor(unsigned char data1, unsigned char data2)
{
    unsigned int i, j;

    // 差了42个点位
    BlockWrite(40, COL - 258 - 1, 40, ROW - 414 - 1);

    digitalWrite(LCD_CS, LOW);

    spi_write(0x32);
    spi_write(0x00);
    spi_write(0x2C);
    spi_write(0x00);

    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL; j++)
        {
            write_lcd(data1);
            write_lcd(data2);
        }
    }

    digitalWrite(LCD_CS, HIGH);
}

void setup()
{
    // 这里必须进行引脚输出配置
    pinMode(LCD_CS, OUTPUT);
    pinMode(LCD_RST, OUTPUT);
    pinMode(LCD_SCLK, OUTPUT);
    pinMode(LCD_SDIO0, OUTPUT);
    pinMode(LCD_SDIO1, OUTPUT);
    pinMode(LCD_SDIO2, OUTPUT);
    pinMode(LCD_SDIO3, OUTPUT);

    pinMode(LCD_EN, OUTPUT);
    digitalWrite(LCD_EN, HIGH);

    LCD_Init();
}

void loop()
{
    // DispColor(0xF8, 0x00); // RED
    // delay(500);
    // DispColor(0x07, 0xE0); // GREEN
    // DispColor(0x00, 0x1F);// BLUE
    // delay(500);

    // shizi_mark(); // 十字架配白色
    // RGB();

    // frame();// 边框配黑色
    // delay(500);
    // gray16();
    // delay(500);

    Gray_bar_H16();
    delay(10000);
}