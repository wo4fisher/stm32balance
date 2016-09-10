/*----------------------------------------------------------------------------
 * Name:    Serial.c
 * Purpose: Low Level Serial Routines
 * Note(s): possible defines select the used communication interface:
 *            __DBG_ITM   - ITM SWO interface
 *                        - USART2 interface  (default)
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2014 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "stm32f10x.h"                  // Device header
#include "Serial.h"

/*----------------------------------------------------------------------------
 Define  USART
 *----------------------------------------------------------------------------*/
#define USARTx  USART1

/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
 
void SER_Init(u32 pclk2, u32 bound) {
//-------------------------------------------------------------------------------	
#if 1
//-------------------------------------------------------------------------------		
	RCC->APB2ENR |= 1<<2;    //使能PORTA口时钟  
	RCC->APB2ENR |= 1<<14;   //使能串口时钟 
	
	GPIOA->CRH &= 0XFFFFF00F; //IO状态设置
	GPIOA->CRH |= 0X000008B0; //IO状态设置
		  
	RCC->APB2RSTR |= 1<<14;   //复位串口1
	RCC->APB2RSTR &= ~(1<<14);//停止复位	   	   
	
 	USART1->BRR = __USART_BRR(pclk2*1000000, bound); // 波特率设置	 
	USART1->CR1 |= 0X200C;  //1位停止,无校验位.

//-------------------------------------------------------------------------------	
#else
//-------------------------------------------------------------------------------	
	
  RCC->APB2ENR |=  (   1ul <<  2);         /* Enable GPIOA clock              */
  RCC->APB1ENR |=  (   1ul << 17);         /* Enable USART#2 clock            */

  /* Configure PA3 to USART2_RX, PA2 to USART2_TX */
  RCC->APB2ENR |=  (   1ul <<  0);         /* enable clock Alternate Function */
  AFIO->MAPR   &= ~(   1ul <<  3);         /* clear USART2 remap              */

  GPIOA->CRL   &= ~(0xFFul <<  8);         /* clear PA2, PA3                  */
  GPIOA->CRL   |=  (0x0Bul <<  8);         /* USART2 Tx (PA2) output push-pull*/
  GPIOA->CRL   |=  (0x04ul << 12);         /* USART2 Rx (PA3) input floating  */

  USARTx->BRR  = __USART_BRR(72000000ul, 115200ul);  /* 115200 baud @ 72MHz   */
  USARTx->CR3   = 0x0000;                  /* no flow control                 */
  USARTx->CR2   = 0x0000;                  /* 1 stop bit                      */
  USARTx->CR1   = ((   1ul <<  2) |        /* enable RX                       */
                   (   1ul <<  3) |        /* enable TX                       */
                   (   0ul << 12) |        /* 1 start bit, 8 data bits        */
                   (   1ul << 13) );       /* enable USART                    */
//-------------------------------------------------------------------------------										 
#endif
//-------------------------------------------------------------------------------	
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int SER_PutChar (int ch) {
  while (!(USARTx->SR & 0x0080));
  USARTx->DR = (ch & 0xFF);
  return (ch);
}

/*----------------------------------------------------------------------------
  Read character from Serial Port
 *----------------------------------------------------------------------------*/
int SER_GetChar (void) {
  if (USARTx->SR & 0x0020)
    return (USARTx->DR);
  return (-1);
}

//------------------------------------------------------------------------------

void SER_Put(unsigned char data)
{
	USARTx->DR = data;
	while((USARTx->SR&0x40)==0);	
}

int SER_Get(unsigned char *data)
{
	if (USARTx->SR & 0x0020){
		*data = (unsigned char)USARTx->DR;
		return 0;
	}
  return (-1);
}

