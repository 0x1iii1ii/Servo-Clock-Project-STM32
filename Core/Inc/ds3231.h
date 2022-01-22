/*
 * ds3231.h
 *
 *  Created on: Nov 1, 2021
 *      Author: 0xliilii
 */

#ifndef INC_DS3231_H_
#define INC_DS3231_H_

#include "stm32f4xx_hal.h"
#include "i2c.h"

#define DS3231_I2C					hi2c1 //Can be change to your desire I2C port.

// i2c slave address of the DS3231 chip
#define DS3231_I2C_ADDR             0x68 << 1 // 7bit addr I2C, 1bit LSB for R/W

// timekeeping registers
#define DS3231_TIME_CAL_ADDR        0x00
#define DS3231_CONTROL_ADDR         0x0E
#define DS3231_STATUS_ADDR          0x0F
#define DS3231_AM_PM_ADDR			0x02
const uint8_t DS3231_SET_12MODE = 0x4;//bit 6 HIGH
struct ts {
    uint8_t sec;         /* seconds */
    uint8_t min;         /* minutes */
    uint8_t hour;        /* hours */
    uint8_t dweek;       /* days of the week */
    uint8_t dmonth;      /* days of the month */
    int16_t month;       /* months */
    uint8_t year;        /* years */
};

void DS3231_setTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dweek, uint8_t dmonth, uint8_t month, uint8_t year);
void DS3231_getTime(struct ts *t);
void DS3231_set12HourMode();

#endif /* INC_DS3231_H_ */
