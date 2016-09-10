#include "exti.h"
#include "sys.h"

//#define INT PBin(5)   //PB5连接到MPU6050的中断引脚

/**************************************************************************
函数功能：外部中断初始化
入口参数：无
返回  值：无 
**************************************************************************/

void EXTI_Init(void)
{
	RCC->APB2ENR |= 1<<3;    						//使能PORTB时钟	   	 
	GPIOB->CRL &= 0XFF0FFFFF; 
	GPIOB->CRL |= 0X00800000;						//PB5上拉输入
  GPIOB->ODR |= 1<<5;      						//PB5上拉	
	Ex_NVIC_Config(GPIO_B,5,FTIR);			//下降沿触发
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2); 	//抢占2，子优先级1，组2
}

