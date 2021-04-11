/*
 The second programming assignment of the 2015 S&I Embedded Seminar 
 Make semi_digital clock by using timer interrupt 

invented by HT
*/

#include "stm32f10x.h"
#include "GPIO_configure.h"
#include "MOTOR_configure.h"
#include "CAM_configure.h"
#include "OLED.h"
#include "font_8.h"
#include "utils.h"
#include "System_Config.h"

extern volatile uint16_t CAM_BUFFER[128];
extern volatile uint16_t CAM_DATA_PRE_1[128];
extern volatile uint16_t CAM_DATA_MEAN[128];
extern volatile uint16_t CAM_DATA_RECT[128]; 
extern volatile uint16_t CAM_DATA_NORMALIZED[128];

int i, max=0, min=0xffff, mode=0;
volatile uint8_t pageIndex = 0;

int main(void)
{
	System_ConfigPLL();		// Overclock this system 8Mhz to 64Mhz
	SysTick_Config(64000);	// System is 64Mhz // 64000 tick -> 1ms period to call SysTick interrup

	GPIO_Configuration();
	OLED_Configuration();
	MOTOR_Configuration();
	CAM_CLK_Configuration();
	CAM_ADC_Configuration();
	
	SYNC_ON;
	
	OLED_begin(SWITCH_CAP_VCC);

	int a=3;
	
	while (1)
	{

		LED_ON();
		
		
		if(BUTTON1_PRESS)
		{
			LED_OFF();
			Delay_ms(100);
		}
		
		OLED_clearDisplay();
		OLED_drawInt(10,10,FONT_8, a);
		OLED_display();	

		Motor_Go(350,350);
	}
}								   

void SysTick_Handler(void)		// 1ms Systick
{	 
	Utils_Task(1000);
}





