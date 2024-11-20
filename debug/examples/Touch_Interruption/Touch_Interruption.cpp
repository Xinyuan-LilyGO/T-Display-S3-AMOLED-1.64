/*
 * @Description: None
 * @version: V1.0.0
 * @Author: None
 * @Date: 2023-08-29 13:34:55
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-01-23 09:43:08
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "pin_config.h"

void setup()
{
    Serial.begin(115200);

    pinMode(TP_INT, INPUT_PULLUP);

    pinMode(TP_RST, OUTPUT);
    digitalWrite(TP_RST, HIGH);
    delay(1);
    digitalWrite(TP_RST, LOW);
    delay(20);
    digitalWrite(TP_RST, HIGH);
    delay(50);

    // digitalWrite(TP_RST, HIGH);
    // delay(100);
    // digitalWrite(TP_RST, LOW);
    // delay(10);
    // digitalWrite(TP_RST, HIGH);
    // delay(200);
}

void loop()
{
    Serial.printf("TP_INT Status:%d\n", digitalRead(TP_INT));

    delay(100);
}