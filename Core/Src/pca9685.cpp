/*
 * pca9685.c
 *
 *  Created on: 20.01.2019
 *      Author: Mateusz Salamon
 *		mateusz@msalamon.pl
 *
 *      Website: https://msalamon.pl/nigdy-wiecej-multipleksowania-na-gpio!-max7219-w-akcji-cz-3/
 *      GitHub:  https://github.com/lamik/Servos_PWM_STM32_HAL
 */

#include "main.h"
#include "i2c.h"

#include "pca9685.h"
#include "math.h"

//I2C_HandleTypeDef *_hi2c;
pca8586::pca8586(uint8_t addr , I2C_HandleTypeDef *hi2c){
	_hi2c = hi2c;
	__ADDR = addr;
}

PCA9685_STATUS pca8586::Init()
{
	//SoftwareReset();
#ifdef PCA9685_SERVO_MODE
	SetPwmFrequency(50);
#else
	SetPwmFrequency(1000);
#endif
	AutoIncrement(1);

	return PCA9685_OK;
}

PCA9685_STATUS pca8586::SetBit(uint8_t Register, uint8_t Bit, uint8_t Value)
{
	uint8_t tmp;
	if(Value) Value = 1;

	if(HAL_OK != HAL_I2C_Mem_Read(_hi2c, __ADDR, Register, 1, &tmp, 1, 10))
	{
		return PCA9685_ERROR;
	}
	tmp &= ~((1<<PCA9685_MODE1_RESTART_BIT)|(1<<Bit));
	tmp |= (Value&1)<<Bit;

	if(HAL_OK != HAL_I2C_Mem_Write(_hi2c, __ADDR, Register, 1, &tmp, 1, 10))
	{
		return PCA9685_ERROR;
	}

	return PCA9685_OK;
}

PCA9685_STATUS pca8586::SoftwareReset(void)
{
	uint8_t cmd = 0x6;
	if(HAL_OK == HAL_I2C_Master_Transmit(_hi2c, 0x00, &cmd, 1, 10))
	{
		return PCA9685_OK;
	}
	return PCA9685_ERROR;
}

PCA9685_STATUS pca8586::SleepMode(uint8_t Enable)
{
	return SetBit(PCA9685_MODE1, PCA9685_MODE1_SLEEP_BIT, Enable);
}

PCA9685_STATUS pca8586::RestartMode(uint8_t Enable)
{
	return SetBit(PCA9685_MODE1, PCA9685_MODE1_RESTART_BIT, Enable);
}

PCA9685_STATUS pca8586::AutoIncrement(uint8_t Enable)
{
	return SetBit(PCA9685_MODE1, PCA9685_MODE1_AI_BIT, Enable);
}

PCA9685_STATUS pca8586::SubaddressRespond(SubaddressBit Subaddress, uint8_t Enable)
{
	return SetBit(PCA9685_MODE1, Subaddress, Enable);
}

PCA9685_STATUS pca8586::AllCallRespond(uint8_t Enable)
{
	return SetBit(PCA9685_MODE1, PCA9685_MODE1_ALCALL_BIT, Enable);
}

//
//	Frequency - Hz value
//
PCA9685_STATUS pca8586::SetPwmFrequency(uint16_t Frequency)
{
	float PrescalerVal;
	uint8_t Prescale;

	if(Frequency >= 1526)
	{
		Prescale = 0x03;
	}
	else if(Frequency <= 24)
	{
		Prescale = 0xFF;
	}
	else
	{
		PrescalerVal = (25000000 / (4096.0 * (float)Frequency)) - 1;
		Prescale = floor(PrescalerVal + 0.5);
	}

	//
	//	To change the frequency, PCA9685 have to be in Sleep mode.
	//
	SleepMode(1);
	HAL_I2C_Mem_Write(_hi2c, __ADDR, PCA9685_PRESCALE, 1, &Prescale, 1, 10); // Write Prescale value
	SleepMode(0);
	RestartMode(1);
	return PCA9685_OK;
}

PCA9685_STATUS pca8586::SetPwm(uint8_t Channel, uint16_t OnTime, uint16_t OffTime)
{
	uint8_t RegisterAddress;
	uint8_t Message[4];

	RegisterAddress = PCA9685_LED0_ON_L + (4 * Channel);
	Message[0] = OnTime & 0xFF;
	Message[1] = OnTime>>8;
	Message[2] = OffTime & 0xFF;
	Message[3] = OffTime>>8;

	if(HAL_OK != HAL_I2C_Mem_Write(_hi2c, __ADDR, RegisterAddress, 1, Message, 4, 10))
	{
		return PCA9685_ERROR;
	}

	return PCA9685_OK;
}

PCA9685_STATUS pca8586::SetPin(uint8_t Channel, uint16_t Value, uint8_t Invert)
{
  if(Value > 4095) Value = 4095;

  if (Invert) {
    if (Value == 0) {
      // Special value for signal fully on.
      return SetPwm(Channel, 4096, 0);
    }
    else if (Value == 4095) {
      // Special value for signal fully off.
    	return SetPwm(Channel, 0, 4096);
    }
    else {
    	return SetPwm(Channel, 0, 4095-Value);
    }
  }
  else {
    if (Value == 4095) {
      // Special value for signal fully on.
    	return SetPwm(Channel, 4096, 0);
    }
    else if (Value == 0) {
      // Special value for signal fully off.
    	return SetPwm(Channel, 0, 4096);
    }
    else {
    	return SetPwm(Channel, 0, Value);
    }
  }
}

#ifdef PCA9685_SERVO_MODE
PCA9685_STATUS pca8586::SetServoAngle(uint8_t Channel, float Angle)
{
	float Value;
	servo_angle[Channel] = Angle;
	if(Angle < MIN_ANGLE) Angle = MIN_ANGLE;
	if(Angle > MAX_ANGLE) Angle = MAX_ANGLE;

	Value = (Angle - MIN_ANGLE) * ((float)SERVO_MAX - (float)SERVO_MIN) / (MAX_ANGLE - MIN_ANGLE) + (float)SERVO_MIN;

	return SetPin(Channel, (uint16_t)Value, 0);
}

float pca8586::GetServoAngle(uint8_t Channel){
	return servo_angle[Channel];
}



#endif


