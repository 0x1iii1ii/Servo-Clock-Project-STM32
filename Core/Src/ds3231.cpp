/*
 * ds3231.c
 *
 *  Created on: Nov 1, 2021
 *      Author: 0xliilii
 */
#include "ds3231.h"

I2C_HandleTypeDef *__hi2c;

//converter BCD & DEC
uint8_t decToBcd(const uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

uint8_t bcdToDec(const uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}

//set time
void DS3231_setTime (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dweek, uint8_t dmonth, uint8_t month, uint8_t year)
{
	uint8_t set_time[7];

	// get input and convert to bcd
	set_time[0] = decToBcd(sec);
	set_time[1] = decToBcd(min);
	set_time[2] = decToBcd(hour);
	set_time[3] = decToBcd(dweek);
	set_time[4] = decToBcd(dmonth);
	set_time[5] = decToBcd(month);
	set_time[6] = decToBcd(year);

	// write time to reg, start from 0x00h
	HAL_I2C_Mem_Write(__hi2c, DS3231_I2C_ADDR, DS3231_TIME_CAL_ADDR, 1, set_time, 7, 1000);
}
//get time from reg
void DS3231_getTime (struct ts *t)
{
	uint8_t get_time[7];
	// read time from reg, start from 0x00h
	HAL_I2C_Mem_Read(__hi2c, DS3231_I2C_ADDR, DS3231_TIME_CAL_ADDR, 1, get_time, 7, 1000);

	t->sec = bcdToDec(get_time[0]);
	t->min = bcdToDec(get_time[1]);
	t->hour = bcdToDec(get_time[2]);
	t->dweek = bcdToDec(get_time[3]);
	t->dmonth = bcdToDec(get_time[4]);
	t->month = bcdToDec(get_time[5]);
	t->year = bcdToDec(get_time[6]);
}
void DS3231_set12HourMode()
{
	HAL_I2C_Mem_Write(__hi2c, DS3231_I2C_ADDR, DS3231_AM_PM_ADDR, 1, (uint8_t*)DS3231_SET_12MODE, 1, 1000);
}
//I2C_HandleTypeDef *_hi2c;
void DS3231_IIC(I2C_HandleTypeDef *hi2c){
	__hi2c = hi2c;
}
