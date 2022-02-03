/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ds3231.h"
#include "pca9685.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PCA9685_SERVO_MODE
#define sleepDigits 11
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//times
struct ts time;
//PWM on and off
int hourOFF[16] = {460,445,445,444,445,460,460,0,445,455,455,440,460,445,445,0};
int hourON[16]	= {266,255,250,245,260,266,271,0,250,265,260,250,280,250,260,0};
int minOFF[16]	= {450,455,435,460,445,455,435,0,440,460,430,440,465,445,435,0};
int minON[16]	= {252,263,252,279,248,260,243,0,253,280,245,245,282,260,240,0};
//Position values for each digit from 0 to 9 and ---- (sleep mode)
int digits[11][7] = {{1,1,1,1,1,1,0},{0,1,1,0,0,0,0},{1,1,0,1,1,0,1},{1,1,1,1,0,0,1},{0,1,1,0,0,1,1},
    {1,0,1,1,0,1,1},{1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1},{0,0,0,0,0,0,0}};

int swTens, prevSwTens, sw, prevSw;
int secTens, prevSecTens, sec, prevSec;
int minTens, prevMinTens, min, prevMin;
int hourTens, prevHourTens, hour, prevHour;
int midOffset = 100;            //Amount by which adjacent segments to the middle move away when required
int tempHour, tempMin, tempSec;
bool setTimeFlag = true;
bool sleepModeState = false;
bool Switched = false;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
pca8586 PWM_dev2(0x40<<1, &hi2c2); //class minutes digits
pca8586 PWM_dev1(0x41<<1, &hi2c2); //class hours digits
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//sleep mode
void sleepMode()
{
	for (int j = 0; j <15; j++) {
		PWM_dev1.SetPwm(j, 0, hourOFF[j]);
		HAL_Delay(100);
	}
	for (int j = 0; j <15; j++) {
		PWM_dev2.SetPwm(j, 0, minOFF[j]);
		HAL_Delay(100);
	}
}
//update servo segments
//void updateDisplay()
//{
//
//}
//Function to move the middle segments and adjacent ones out of the way
void updateMid()
{
	//----------------------------------------------------//
	// Move adjacent segments for Minute and second units //
	//----------------------------------------------------//

	if (digits[swTens][6] != digits[prevSwTens][6]) {
		if (digits[prevSwTens][1] == 1)
			PWM_dev2.SetPwm(1, 0, minON[1] + midOffset);
		if (digits[prevSwTens][6] == 1)
			PWM_dev2.SetPwm(5, 0, minOFF[5] - midOffset);
	}
	HAL_Delay(100); //Delay allows adjacent segments to move before moving middle
	if (digits[swTens][6] == 1)  //Move Minute units middle segment if required
		PWM_dev2.SetPwm(6, 0, minON[6]);
	else
		PWM_dev2.SetPwm(6, 0, minOFF[6]);

	if (digits[sw][6] != digits[prevSw][6]) //Move adjacent segments for Minute tens
			{
		if (digits[prevSw][1] == 1)
			PWM_dev2.SetPwm(9, 0, minON[9] + midOffset);
		if (digits[prevSw][6] == 1)
			PWM_dev2.SetPwm(13, 0, minOFF[13] - midOffset);
	}
	HAL_Delay(100); //Delay allows adjacent segments to move before moving middle
	if (digits[sw][6] == 1)       //Move Minute tens middle segment if required
		PWM_dev2.SetPwm(14, 0, minON[14]);
	else
		PWM_dev2.SetPwm(14, 0, minOFF[14]);

	//====================================================================//

	//-----------------------------------------//
	//  Move adjacent segments for Hour units  //
	//-----------------------------------------//

	if (digits[hourTens][6] != digits[prevHourTens][6]) //Move adjacent segments for Minute units
			{
		if (digits[prevHourTens][1] == 1)
			PWM_dev1.SetPwm(1, 0, hourON[1] + midOffset);
		if (digits[prevHourTens][6] == 1)
			PWM_dev1.SetPwm(5, 0, hourOFF[5] - midOffset);
	}
	HAL_Delay(100); //Delay allows adjacent segments to move before moving middle
	if (digits[hourTens][6] == 1) //Move Minute units middle segment if required
		PWM_dev1.SetPwm(6, 0, hourON[6]);
	else
		PWM_dev1.SetPwm(6, 0, hourOFF[6]);

	if (digits[hour][6] != digits[prevHour][6]) //Move adjacent segments for Minute tens
			{
		if (digits[prevHour][1] == 1)
			PWM_dev1.SetPwm(9, 0, hourON[9] + midOffset);
		if (digits[prevHour][6] == 1)
			PWM_dev1.SetPwm(13, 0, hourOFF[13] - midOffset);
	}
	HAL_Delay(100); //Delay allows adjacent segments to move before moving middle
	if (digits[hour][6] == 1)      //Move Minute tens middle segment if required
		PWM_dev1.SetPwm(14, 0, hourON[14]);
	else
		PWM_dev1.SetPwm(14, 0, hourOFF[14]);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  //Initial PWM drivers
  PWM_dev2.Init();
  PWM_dev1.Init();
  DS3231_IIC(&hi2c1);
  //off all digits
	for (int j = 0; j <15; j++) {
		PWM_dev1.SetPwm(j, 0, hourOFF[j]);
		HAL_Delay(80);
	}
	for (int j = 0; j <15; j++) {
		PWM_dev2.SetPwm(j, 0, minOFF[j]);
		HAL_Delay(80);
	}
	//set times (only once)
	//DS3231_setTime(55, 59, 23, 3, 8, 12, 21);
	DS3231_getTime(&time);//Get time
	//char data[20];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

		DS3231_getTime(&time);   //Get time
		tempHour = time.hour; 	 //Get the hours and save to variable
		tempMin = time.min;      //Get the minutes and save to variable
		tempSec = time.sec;      //Get the second and save to variable
		//switch minute to second at 11:58 for 2 minutes
		if (tempHour == 23 && tempMin >= 58 && tempMin <= 59) Switched = true;
		else Switched = false;
		//let it sleep from 7:00 PM to 6 AM
		if (tempHour >= 7 && tempHour <= 18) sleepModeState = true;
		else sleepModeState = false;
		//12 hour Mode
		if (tempHour > 12)
			tempHour = tempHour - 12;
		if(tempHour == 0)
			tempHour = 12;
		//Split hours into two digits, tens and units
		hourTens = tempHour / 10;
		hour = tempHour % 10;
		//Split minutes into two digits, tens and units
		minTens = tempMin / 10;
		min = tempMin % 10;
		//Split minutes into two digits, tens and units
		secTens = tempSec / 10;
		sec = tempSec % 10;

		if (Switched){
			swTens = secTens;
			sw = sec;
		}
		else{
			swTens = minTens;
			sw = min;
		}
		if (sleepModeState){
			sleepMode();
		}
		else{
			updateMid();
			for (int i = 0; i < 6; i++)             			//Move the remaining 6 segments
					{
				if (digits[hourTens][i] == 1)                 	//Update the hour tens
					PWM_dev1.SetPwm(i, 0, hourON[i]);
				else
					PWM_dev1.SetPwm(i, 0, hourOFF[i]);
				if (digits[hour][i] == 1)               		//Update the hour units
					PWM_dev1.SetPwm(i + 8, 0, hourON[i + 8]);
				else
					PWM_dev1.SetPwm(i + 8, 0, hourOFF[i + 8]);
				if (digits[swTens][i] == 1)             		//Update the minute tens
					PWM_dev2.SetPwm(i, 0, minON[i]);
				else
					PWM_dev2.SetPwm(i, 0, minOFF[i]);
				if (digits[sw][i] == 1)           				//Update the minute units
					PWM_dev2.SetPwm(i + 8, 0, minON[i + 8]);
				else
					PWM_dev2.SetPwm(i + 8, 0, minOFF[i + 8]);
			}
		}
			prevSwTens = swTens;
			prevSw = sw;
			prevHourTens = hourTens;
			prevHour = hour;
//	  sprintf(data,"H10: %d\n",hourTens);
//	  HAL_UART_Transmit(&huart1, (uint8_t*)data, sizeof(data), 1000);
//	  sprintf(data,"H: %d\n",hour);
//	  HAL_UART_Transmit(&huart1, (uint8_t*)data, sizeof(data), 1000);
//	  sprintf(data,"hour: %d\n",time.hour);
//	  HAL_UART_Transmit(&huart1, (uint8_t*)data, sizeof(data), 1000);
	  //HAL_UART_Transmit(&huart1, (uint8_t*)time.min, sizeof(time.min), 1000);
	  //HAL_UART_Transmit(&huart1, (uint8_t*)time.sec, sizeof(time.sec), 1000);
	  //HAL_Delay(500);
	  //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	  //HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
